#ifndef HOTKEY_MGR_H
#define HOTKEY_MGR_H
#include "singleton.h"
#include <QtGui\QKeySequence>
#include "QHash"
#include "QReadWriteLock"

#define ENABLE_HOTKEY  "enable_hotkey"

class hotkey_mgr : public Singleton<hotkey_mgr>
{
public:
	enum eHotKeyState{
		// 快捷健/热键注册状态
		eDisable=-3,    // 已被禁用
		eInvalid,       // 按键非法(没有modifier单独按键(数字和字母)不允许)
		eUnkown,        // 未知错误
		eNone,          // 正常
		eExist,         // 快捷键已存在
		eConflict       // 系统热键注册失败(被占用, 冲突)
	};

	enum eHotKeyType{
		eInvalidType = -1,
		// 全局热键
		eGlobalHotKey =0,
		eGlobalHotKey1,
		eGlobalHotKey2,
		eGlobalHotKey3,
		eGlobalHotKey4,
		eGlobalHotKey5,
		eGlobalHotKey6,
		eGlobalHotKey7,
		eGlobalHotKey8,
		eGlobalHotKey9,
		eGlobalHotKeyCount = eGlobalHotKey+10
	};
	
	// 获取冲突的热键数量
	int getConfictedHotKeyNum();
	/*
	 * 启用/禁用热键
	 */
	void enableGlobalHotKeys(bool enable = true);
	bool isEnableGlobalHotKeys(){return m_enble;};
	/*
	 * 检查热键类型是否已经注册
	 */
	bool isHotKeyTypeExist(eHotKeyType type);
	/*
	 * 检查热键状态, keysq为空时, 检查type类型的热键当前状态, 不为空时检查type类型的新按键是否已存在
	 */
	eHotKeyState checkHotKeyState(eHotKeyType type, const QKeySequence& keysq=QKeySequence(0));
	/*
	 * 注册快捷键, 全局类型(type<eGloableHotKeyCount)的自动注册系统热键, 非全局类型仅存储
	 * e.g. QKeySequence(Qt::ControlModifier | Qt::ShiftModifier | Qt::Key_O)
	 */
	eHotKeyState registerHotKey(eHotKeyType type, const QKeySequence& keysq);
	/*
	 * 反注册快捷键, 全局类型(type<eGloableHotKeyCount)的自动反注册系统热键, 非全局类型仅存储
	 */
	bool unRegisterHotKey(eHotKeyType type);
	/*
	 *  通过 QKeySequence 得到 Key 和 modifiers
	 */
	void transKeySeq2KeyMods( const QKeySequence &keysq, Qt::Key &key, Qt::KeyboardModifiers &mods );
	/*
	 * 根据类型获取按键字符串, 未注册则返回 "NULL"
	 * forToolTip == true 时, 仅当已注册且状态为 eNone时返回字符串, 否则为空
	 */
	QString getHotKeyStringByType(eHotKeyType type, bool forToolTip = false);
	/*
	 * 获取按键字符串
	 */
	QString getHotKeyStringBySq(const QKeySequence& keysq);
	/*
	 *  通过 WM_HOTKEY lparam(包含 Key 和 Mods)获取热键类型, 若不存在返回 eInvalidType
	 */
	eHotKeyType getHotKeyTypeByNativeParam( long lParam );
	/*
	 * 通过按键获取类型 不存在返回 eInvalidType
	 */ 
    eHotKeyType getHotKeyTypeBySq(const QKeySequence& keysq);
	/*
	 *  通过类型获取按键 不存在返回 false
	 */ 
	bool getHotKeySeq(eHotKeyType type, QKeySequence& keysq);
	/*
	 *  通过类型获取按键 不存在或状态不为eNone返回 QKeySequence()
	 */ 
	QKeySequence getHotKeySeq(eHotKeyType type);
	/*
	 * 恢复默认按键, 如果默认按键有冲突返回false, 全部ok返回true
	 */
	bool defaultHotKeys();
    /*
	 * 该类型是合法(是否曾在默认值)
	 */
    bool isHotKeyTypeValid(eHotKeyType type){return m_cacheDefaultHotkeys.contains(type);};
protected:
	hotkey_mgr();
	~hotkey_mgr();
	friend class Singleton<hotkey_mgr>;
protected:
	// 初始化默认按键, 加载配置文件和注册系统热键
	bool init();
	bool loadConfig();
	void initDefaultHotKeys(void);
	qint32 nativeModifiers(Qt::KeyboardModifiers modifiers);
	qint32 nativeKeycode(Qt::Key key);

	bool getNativeKeyAndMods( eHotKeyType type, quint32 &nativeKey, quint32 &nativeMods );
	bool getNativeKeyAndModsBySq( const QKeySequence &keysq, quint32 &nativeKey, quint32 &nativeMods );

	// 针对小键盘数字+shift转义, 防止出现热键注册成功但是无法捕获 WM_HOTKEY
	void ShiftKeysAndMods( Qt::Key &key, Qt::KeyboardModifiers &mods );
	bool registerGlobalHotKey(eHotKeyType type, const QKeySequence& keysq);
	bool unRegisterGlobalHotKey(eHotKeyType type);
	bool setState(eHotKeyType type, eHotKeyState state);
	QKeySequence processSpecialKeys( QKeySequence newKeysq );
	bool isNumPadFuncKey( Qt::Key key );
	

private:
	QHash <eHotKeyType, QPair<QKeySequence, eHotKeyState>> m_cacheHotkeys;
	QReadWriteLock                    m_cacheLock;
	QHash <eHotKeyType, QKeySequence> m_cacheDefaultHotkeys;
	bool  m_enble;
	bool  m_bWorking;
};

#endif // HOTKEY_MGR_H
