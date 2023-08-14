#include "hotkey_mgr.h"
#include <Windows.h>
#include <assert.h>

hotkey_mgr::hotkey_mgr()
{
	m_bWorking = false;
	init();
}

hotkey_mgr::~hotkey_mgr()
{

}

bool hotkey_mgr::init()
{
	if (m_cacheDefaultHotkeys.size()>0)
	{
		// 已经初始化过
		return true;
	}
	initDefaultHotKeys();
	bool ret = loadConfig();

	// 如果配置文件为空,使用默认值
    if (m_cacheHotkeys.size() < 1 )
	{
		ret = defaultHotKeys();
	}

	return ret;
}

bool hotkey_mgr::isHotKeyTypeExist(eHotKeyType type)
{
	QReadLocker lock(&m_cacheLock);	
	return m_cacheHotkeys.contains(type);
}

hotkey_mgr::eHotKeyState hotkey_mgr::checkHotKeyState(eHotKeyType type, const QKeySequence& keysq)
{
	eHotKeyState state = eNone;
	bool bOlnyCheckType = (keysq == QKeySequence(0));
	if (!bOlnyCheckType)
	{
		Qt::KeyboardModifiers mods;
		Qt::Key key;
		transKeySeq2KeyMods(keysq, key, mods);
		bool isNumber = key >= Qt::Key_0 && key <= Qt::Key_9;
		bool isLetter = key >= Qt::Key_A && key <= Qt::Key_Z;
		bool isModifiers = (key == Qt::Key_Shift||key == Qt::Key_Control||key == Qt::Key_Alt);
		// 单独按键 纯数字和字母不允许设置热键, 发信只能使用组合键
		if (!nativeKeycode(key) || isModifiers ||
			(mods == 0 && (isNumber || isLetter)) ||
			(mods == Qt::NoModifier ||mods == Qt::KeypadModifier))
		{
			return eInvalid;
		}
	}
	auto newKeysq = processSpecialKeys(keysq);
	QReadLocker lock(&m_cacheLock);
	foreach(auto ipair, m_cacheHotkeys)
	{
		auto curType = m_cacheHotkeys.key(ipair);
		if (curType == type && bOlnyCheckType)
		{
			state = ipair.second; // 返回该类型当前状态
			break;
		}
		else if (!bOlnyCheckType && curType != type && newKeysq == ipair.first)
		{
			state = eExist; // 检查新按键是否已被其他类型热键占用
			break;
		}
	}

	return state;
}

hotkey_mgr::eHotKeyState hotkey_mgr::registerHotKey(eHotKeyType type, const QKeySequence& keysq)
{
	assert(type > eInvalidType && type != eGlobalHotKeyCount);
	eHotKeyState state = eNone;

	QKeySequence orgKeysq;	
	QKeySequence newKeysq = processSpecialKeys(keysq);

        registerGlobalHotKey(type, newKeysq);
            QWriteLocker lock(&m_cacheLock);
            m_cacheHotkeys.insert(type, QPair<QKeySequence, eHotKeyState>(newKeysq, state));

		

	return state;
}

bool hotkey_mgr::unRegisterHotKey(eHotKeyType type)
{
	assert(type > eInvalidType && type != eGlobalHotKeyCount);
	if (!unRegisterGlobalHotKey(type))
	{
		return false;
	}

	if (isHotKeyTypeExist(type))
	{
        {
            QWriteLocker lock(&m_cacheLock);
            m_cacheHotkeys.remove(type);
        }
		
		// save to file...
		return true;
	}

	return false;
}

bool hotkey_mgr::registerGlobalHotKey(eHotKeyType type, const QKeySequence& keysq)
{
	if (type > eGlobalHotKeyCount)
	{
		return true;
	}

	// 如果该功能已经有了热键, 取消注册旧的, 重新注册新的
	if (isHotKeyTypeExist(type))
	{
		if (!unRegisterGlobalHotKey(type))
		{
			//return false;
		}
	}
	quint32 nativeKey = 0;
	quint32 nativeMods = 0;
	if (!getNativeKeyAndModsBySq(keysq, nativeKey, nativeMods))
	{
		return false;
	}

	assert(IsGUIThread(false));
	if (!RegisterHotKey(0,nativeMods ^ nativeKey, nativeMods, nativeKey))
	{
		return false;
	}
	return true;//registerHotKey(type, keysq);
}

bool hotkey_mgr::getNativeKeyAndMods( eHotKeyType type, quint32 &nativeKey, quint32 &nativeMods )
{
	QKeySequence keysq;
	if (!getHotKeySeq(type, keysq))
	{
		return false;
	}
	return getNativeKeyAndModsBySq(keysq, nativeKey, nativeMods);
	
}

bool hotkey_mgr::getNativeKeyAndModsBySq( const QKeySequence &keysq, quint32 &nativeKey, quint32 &nativeMods )
{
	Qt::Key key(Qt::Key_unknown);
	Qt::KeyboardModifiers mods(0);
	transKeySeq2KeyMods(keysq, key, mods);
	nativeKey = nativeKeycode(key);
	nativeMods = nativeModifiers(mods);
	if (nativeKey >= '0' && nativeKey <= '9' &&
		(mods & Qt::KeypadModifier/* 数字小键盘 */))
	{
		nativeKey = VK_NUMPAD0 + (nativeKey - '0');
	}
	if (nativeKey == 0)
	{
	}
	
	return nativeKey != 0;
}

bool hotkey_mgr::unRegisterGlobalHotKey(eHotKeyType type)
{
	if (type > eGlobalHotKeyCount)
	{
		return true;
	}

	quint32 nativeKey = 0;
	quint32 nativeMods = 0;
	if (!getNativeKeyAndMods(type, nativeKey, nativeMods))
	{
		return false;
	}
	assert(IsGUIThread(false));
	if (isHotKeyTypeExist(type)&& UnregisterHotKey(0,nativeMods ^ nativeKey))
	{
		return true;//unRegisterHotKey(type);
	}

	return false;
}

bool hotkey_mgr::getHotKeySeq(eHotKeyType type, QKeySequence& keysq)
{
	QReadLocker lock(&m_cacheLock);
	if (m_cacheHotkeys.contains(type))
	{
		keysq = m_cacheHotkeys.value(type).first;
		return true;
	}

	return false;
}

QKeySequence hotkey_mgr::getHotKeySeq(eHotKeyType type)
{
	QKeySequence keysq;
	if (checkHotKeyState(type)==eNone)
	{
		getHotKeySeq(type, keysq);
	}
	
	return keysq;
}

void hotkey_mgr::transKeySeq2KeyMods( const QKeySequence &keysq, Qt::Key &key, Qt::KeyboardModifiers &mods )
{
	assert(keysq.count()<=1);
	Qt::KeyboardModifiers allMods = Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier | /*Qt::MetaModifier | //忽略按下的meta键 */
		Qt::KeypadModifier/* 数字小键盘 */;
//	bool isempty = keysq.isEmpty();
//	int value = (keysq[0] ^ allMods) & keysq[0];
//	int v0 = keysq[0];
	key = keysq.isEmpty() ? Qt::Key(0) : Qt::Key((keysq[0] ^ allMods) & keysq[0]);
	mods = keysq.isEmpty() ? Qt::KeyboardModifiers(0) : Qt::KeyboardModifiers(keysq[0] & allMods);
	ShiftKeysAndMods(key, mods);
}

void hotkey_mgr::ShiftKeysAndMods( Qt::Key &key, Qt::KeyboardModifiers &mods )
{
	if(0 == (mods & Qt::ShiftModifier))
	{
		return;
	}

	if ((mods & Qt::KeypadModifier)) // shift + 小键盘
	{
		Qt::Key orgKey = key;
		switch(key)
		{
		case Qt::Key_0:		key = Qt::Key_Insert;	break;
		case Qt::Key_1:		key = Qt::Key_End;		break;
		case Qt::Key_2:		key = Qt::Key_Down;		break;
		case Qt::Key_3:		key = Qt::Key_PageDown;	break;
		case Qt::Key_4:		key = Qt::Key_Left;		break;
		case Qt::Key_5:		key = Qt::Key_unknown;	break;
		case Qt::Key_6:		key = Qt::Key_Right;	break;
		case Qt::Key_7:		key = Qt::Key_Home;		break;
		case Qt::Key_8:		key = Qt::Key_Up;		break;
		case Qt::Key_9:		key = Qt::Key_PageUp;	break;
		case Qt::Key_Period: key = Qt::Key_Delete;	break;
		}
		// remove shift if numlocked 
		if (orgKey!=key)
		{
			bool isNumLock = LOBYTE(GetKeyState(VK_NUMLOCK));
			auto needRemoveMods = (isNumLock /*&& orgKey!=key*/) ? (Qt::KeypadModifier | Qt::ShiftModifier) : Qt::KeypadModifier;
			// 去掉小键盘和shift
			mods &=  mods ^ needRemoveMods;
		}
	}
	else
	{
		switch(key)
		{
		case Qt::Key_AsciiTilde : key = Qt::Key_QuoteLeft;  break;  // ~
		case Qt::Key_Exclam:     key = Qt::Key_1;           break;  // !
		case Qt::Key_At:         key = Qt::Key_2;           break;  // @
		case Qt::Key_NumberSign: key = Qt::Key_3;           break;  // #
		case Qt::Key_Dollar:     key = Qt::Key_4;           break;  // $
		case Qt::Key_Percent:    key = Qt::Key_5;           break;  // %
		case Qt::Key_AsciiCircum:key = Qt::Key_6;           break;  // ^
		case Qt::Key_Ampersand:  key = Qt::Key_7;           break;  // &
		case Qt::Key_Asterisk:   key = Qt::Key_8;           break;  // *
		case Qt::Key_ParenLeft:  key = Qt::Key_9;           break;  // (
		case Qt::Key_ParenRight: key = Qt::Key_0;           break;  // )

		case Qt::Key_Underscore: key = Qt::Key_Minus;       break;  // _ -> -
		case Qt::Key_Plus:       key = Qt::Key_Equal;       break;  // + -> =
		case Qt::Key_BraceLeft:  key = Qt::Key_BracketLeft; break;  // { -> [
		case Qt::Key_BraceRight: key = Qt::Key_BracketRight;break;  // } -> ]
		case Qt::Key_Colon:      key = Qt::Key_Semicolon;   break;  // : -> ;
		case Qt::Key_QuoteDbl:   key = Qt::Key_Apostrophe;  break;  // " -> '
		case Qt::Key_Bar:        key = Qt::Key_Backslash;   break;  // | -> '\'
		case Qt::Key_Less:       key = Qt::Key_Comma;       break;  // < -> ,
		case Qt::Key_Greater:    key = Qt::Key_Period;      break;  // > -> .
		case Qt::Key_Question:   key = Qt::Key_Slash;       break;  // ? -> /
		}
	}
}

qint32 hotkey_mgr::nativeModifiers(Qt::KeyboardModifiers modifiers)
{
	// MOD_ALT, MOD_CONTROL, (MOD_KEYUP), MOD_SHIFT, MOD_WIN
	quint32 native = 0;
	if (modifiers & Qt::ShiftModifier)
		native |= MOD_SHIFT;
	if (modifiers & Qt::ControlModifier)
		native |= MOD_CONTROL;
	if (modifiers & Qt::AltModifier)
		native |= MOD_ALT;
	if (modifiers & Qt::MetaModifier)
		native |= MOD_WIN;
	return native;
}

qint32 hotkey_mgr::nativeKeycode(Qt::Key key)
{
	switch (key)
	{
	case Qt::Key_Escape:
		return VK_ESCAPE;
	case Qt::Key_F1:
		return VK_F1;
	case Qt::Key_F2:
		return VK_F2;
	case Qt::Key_F3:
		return VK_F3;
	case Qt::Key_F4:
		return VK_F4;
	case Qt::Key_F5:
		return VK_F5;
	case Qt::Key_F6:
		return VK_F6;
	case Qt::Key_F7:
		return VK_F7;
	case Qt::Key_F8:
		return VK_F8;
	case Qt::Key_F9:
		return VK_F9;
	case Qt::Key_F10:
		return VK_F10;
	case Qt::Key_F11:
		return VK_F11;
	case Qt::Key_F12:
		return VK_F12;
	case Qt::Key_F13:
		return VK_F13;
	case Qt::Key_F14:
		return VK_F14;
	case Qt::Key_F15:
		return VK_F15;
	case Qt::Key_F16:
		return VK_F16;
	case Qt::Key_F17:
		return VK_F17;
	case Qt::Key_F18:
		return VK_F18;
	case Qt::Key_F19:
		return VK_F19;
	case Qt::Key_F20:
		return VK_F20;
	case Qt::Key_F21:
		return VK_F21;
	case Qt::Key_F22:
		return VK_F22;
	case Qt::Key_F23:
		return VK_F23;
	case Qt::Key_F24:
		return VK_F24;
	case Qt::Key_QuoteLeft :
	case Qt::Key_AsciiTilde:
		return VK_OEM_3; /*`~*/

	case Qt::Key_Tab:
	case Qt::Key_Backtab:
		return VK_TAB;
	case Qt::Key_CapsLock :
		return VK_CAPITAL; /*Caps Lock*/

		// numbers
	case Qt::Key_0:
	case Qt::Key_1:
	case Qt::Key_2:
	case Qt::Key_3:
	case Qt::Key_4:
	case Qt::Key_5:
	case Qt::Key_6:
	case Qt::Key_7:
	case Qt::Key_8:
	case Qt::Key_9:
		return key;

	case Qt::Key_Exclam:     // !
		return Qt::Key_1;
		break;
	case Qt::Key_At:         // @
		return Qt::Key_2;
		break;
	case Qt::Key_NumberSign: // #
		return Qt::Key_3;
		break;
	case Qt::Key_Dollar:     // $
		return Qt::Key_4;
		break;
	case Qt::Key_Percent:    // %
		return Qt::Key_5;
		break;
	case Qt::Key_AsciiCircum:// ^
		return Qt::Key_6;
		break;
	case Qt::Key_Ampersand:  // &
		return Qt::Key_7;
		break;
// 	case Qt::Key_Asterisk:   // *
// 		return Qt::Key_8;
// 		break;
	case Qt::Key_ParenLeft:  // (
		return Qt::Key_9;
		break;
	case Qt::Key_ParenRight: // )
		return Qt::Key_0;
		break;

		// letters
	case Qt::Key_A:
	case Qt::Key_B:
	case Qt::Key_C:
	case Qt::Key_D:
	case Qt::Key_E:
	case Qt::Key_F:
	case Qt::Key_G:
	case Qt::Key_H:
	case Qt::Key_I:
	case Qt::Key_J:
	case Qt::Key_K:
	case Qt::Key_L:
	case Qt::Key_M:
	case Qt::Key_N:
	case Qt::Key_O:
	case Qt::Key_P:
	case Qt::Key_Q:
	case Qt::Key_R:
	case Qt::Key_S:
	case Qt::Key_T:
	case Qt::Key_U:
	case Qt::Key_V:
	case Qt::Key_W:
	case Qt::Key_X:
	case Qt::Key_Y:
	case Qt::Key_Z:
		return key;

	//case Qt::Key_Minus:
	case Qt::Key_Underscore:
		return VK_OEM_MINUS; // '-_' any country
	case Qt::Key_Equal:
	//case Qt::Key_Plus:
		return VK_OEM_PLUS;  // '+=' any country
	case Qt::Key_BraceLeft:
	case Qt::Key_BracketLeft:
		return VK_OEM_4;     //  '[{' for US
	case Qt::Key_BraceRight:
	case Qt::Key_BracketRight:
		return VK_OEM_6;     //  ']}' for US
	case Qt::Key_Semicolon:
	case Qt::Key_Colon:
		return VK_OEM_1;     // ';:' for US
	case Qt::Key_Apostrophe:
	case Qt::Key_QuoteDbl:
		return VK_OEM_7;     //  ''"' for US
	case Qt::Key_Backslash:
	case Qt::Key_Bar:
		return VK_OEM_5;     //  '\|' for US
	//case Qt::Key_Comma:
	case Qt::Key_Less:
		return VK_OEM_COMMA;  // ',<' any country
	case Qt::Key_Period:
	case Qt::Key_Greater:
		return VK_OEM_PERIOD; // '.>' any country
	//case Qt::Key_Slash:
	case Qt::Key_Question:
		return VK_OEM_2;      // '/?' for US
	
	case Qt::Key_Backspace:
		return VK_BACK;
	case Qt::Key_Return:
	case Qt::Key_Enter:
		return VK_RETURN;
	case Qt::Key_Insert:
		return VK_INSERT;
	case Qt::Key_Delete:
		return VK_DELETE;
	case Qt::Key_Pause:
		return VK_PAUSE;
	case Qt::Key_Print:
		return VK_PRINT;
	case Qt::Key_Clear:
		return VK_CLEAR;
	case Qt::Key_Home:
		return VK_HOME;
	case Qt::Key_End:
		return VK_END;
	case Qt::Key_Left:
		return VK_LEFT;
	case Qt::Key_Up:
		return VK_UP;
	case Qt::Key_Right:
		return VK_RIGHT;
	case Qt::Key_Down:
		return VK_DOWN;
	case Qt::Key_PageUp:
		return VK_PRIOR;
	case Qt::Key_PageDown:
		return VK_NEXT;
	
	case Qt::Key_Space:
		return VK_SPACE;
	case Qt::Key_Asterisk: // *
		return VK_MULTIPLY;
	case Qt::Key_Plus:
		return VK_ADD;
	case Qt::Key_Comma:
		return VK_SEPARATOR;
	case Qt::Key_Minus:
		return VK_SUBTRACT;
	case Qt::Key_Slash:
		return VK_DIVIDE;

	case Qt::Key_ScrollLock:
		return VK_SCROLL;
	case Qt::Key_NumLock:
		return VK_NUMLOCK;
	

	default:
		return 0;
	}
}

void hotkey_mgr::initDefaultHotKeys()
{

}

bool hotkey_mgr::loadConfig()
{


    return 1;
}

QString hotkey_mgr::getHotKeyStringByType(eHotKeyType type, bool forToolTip/* = false*/)
{
	QKeySequence keysq;
	if (!getHotKeySeq(type, keysq))
	{
		return forToolTip ? QString() : QString("NULL");
	}
	auto state = checkHotKeyState(type);
	QString strKeys = getHotKeyStringBySq(keysq);
	if (forToolTip)
	{
		// 冲突时tooltip不显示热键, tooltip加括号..
		if (state!=eNone)
		{
			strKeys.clear();
		}
		else
		{
			strKeys = QString("(%1)").arg(strKeys);
		}
	}

	return strKeys;
}

QString hotkey_mgr::getHotKeyStringBySq(const QKeySequence& ckeysq)
{
	//QKeySequence((ckeysq[0] ^ Qt::MetaModifier) & ckeysq[0]);
	QKeySequence keysq = processSpecialKeys(ckeysq);
	QString strText;
	Qt::Key key;
	Qt::KeyboardModifiers mods;
	bool isNumpad = (keysq[0] & Qt::KeypadModifier) ==  Qt::KeypadModifier;
	transKeySeq2KeyMods( keysq, key, mods);
	if (mods & Qt::ControlModifier)
	{
		strText += "Ctrl+";
	}
	if (mods & Qt::ShiftModifier)
	{
		strText += "Shift+";
	}
	if (mods & Qt::AltModifier)
	{
		strText += "Alt+";
	}
	if (key == Qt::Key_Return)
	{
		key = Qt::Key_Enter; // "Return"-->"Enter"
	}
	QString strKey = QKeySequence(key).toString(QKeySequence::PortableText);
	
	// 非法字符清空
	if (!nativeKeycode(key))
	{
		strKey.clear();
	}

	// 小键盘数字
	bool isNumber = key >= Qt::Key_0 && key <= Qt::Key_9;
	bool isInNumPad = (key == Qt::Key_Plus || key == Qt::Key_Minus || /* + - * / . */
		key == Qt::Key_Asterisk || key == Qt::Key_Slash || key == Qt::Key_Period);
	if (isNumpad && (isNumber || isInNumPad))
	{
		strKey = QString("Num ") + strKey;
	}
	strText += strKey;

	return strText;
}

hotkey_mgr::eHotKeyType hotkey_mgr::getHotKeyTypeByNativeParam( long lParam )
{
	eHotKeyType type = eInvalidType;
	const quint32 keycode = HIWORD(lParam);
	const quint32 modifiers = LOWORD(lParam) & (MOD_CONTROL|MOD_SHIFT|MOD_ALT);

	QReadLocker lock(&m_cacheLock);
	foreach(auto pair, m_cacheHotkeys)
	{
		quint32 key = 0;
		quint32 mods = 0;
		if (getNativeKeyAndModsBySq(pair.first,key,mods) && 
			key == keycode && mods == modifiers)
		{
			return m_cacheHotkeys.key(pair);
		}
	}

	return type;
}


bool hotkey_mgr::defaultHotKeys()
{
	if (m_bWorking)
	{
		return true;
	}
	m_bWorking = true;

	//enableGlobalHotKeys();

    foreach(auto var, m_cacheDefaultHotkeys)
    {
        auto type = m_cacheDefaultHotkeys.key(var);
        // 和默认值不一样的清空
        if (var != getHotKeySeq(type))
        {
            unRegisterHotKey(type);
        }
    }

	bool ret = true;
	foreach(auto var, m_cacheDefaultHotkeys)
	{
		auto state = registerHotKey(m_cacheDefaultHotkeys.key(var), var);
		if (ret && state != eNone)
		{
			ret = false;
		}
	}
	m_bWorking = false;
	return ret;
}

void hotkey_mgr::enableGlobalHotKeys(bool enable/* = true*/)
{

}

bool hotkey_mgr::setState(eHotKeyType type, eHotKeyState state)
{
	QKeySequence keysq;
	if (!getHotKeySeq(type, keysq))
	{
		// not exist
		return false;
	}	
	QWriteLocker lock(&m_cacheLock);
	m_cacheHotkeys.insert(type, QPair<QKeySequence, eHotKeyState>(keysq, state));
	return true;

}

hotkey_mgr::eHotKeyType hotkey_mgr::getHotKeyTypeBySq(const QKeySequence& keysq)
{
	// Qt::Key_Return --> Qt::Key_Enter
	QKeySequence newKeysq = processSpecialKeys(keysq);
    eHotKeyType ret = eInvalidType;
	QReadLocker lock(&m_cacheLock);
	foreach(auto pair, m_cacheHotkeys)
	{
		if (newKeysq == pair.first)
		{
			return m_cacheHotkeys.key(pair);
		}
	}

    return ret;
}

int hotkey_mgr::getConfictedHotKeyNum()
{
	int nCount = 0;
	QReadLocker lock(&m_cacheLock);
	foreach(auto pair, m_cacheHotkeys)
	{
		if (pair.second == eConflict)
		{
			++nCount;
		}
	}
	return nCount;
}

QKeySequence hotkey_mgr::processSpecialKeys( QKeySequence keysq )
{
	// remove meta modifiers
	QKeySequence newKeysq = QKeySequence((keysq[0] ^ Qt::MetaModifier) & keysq[0]); 
	
	// Qt::Key_Return --> Qt::Key_Enter
	Qt::Key key(Qt::Key_unknown);
	Qt::KeyboardModifiers mods(0);
	transKeySeq2KeyMods(newKeysq, key,mods);
	if ((mods & Qt::KeypadModifier) && isNumPadFuncKey(key))
	{
		mods = (mods^Qt::KeypadModifier)&mods;
	}

	if (key == Qt::Key_Return)
	{
		key = Qt::Key_Enter;
	}
	//QString str = QKeySequence(mods|key).toString();
	return QKeySequence(mods|key);
}

bool hotkey_mgr::isNumPadFuncKey( Qt::Key key )
{
	return (key == Qt::Key_Enter  || key == Qt::Key_Insert ||
		    key == Qt::Key_End    || key == Qt::Key_Down   ||
			key == Qt::Key_PageDown || key == Qt::Key_Left ||
			key == Qt::Key_Right    || key == Qt::Key_Home ||
			key == Qt::Key_Up       ||key == Qt::Key_PageUp||
			key == Qt::Key_Delete);
}
