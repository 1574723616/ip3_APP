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
		// ��ݽ�/�ȼ�ע��״̬
		eDisable=-3,    // �ѱ�����
		eInvalid,       // �����Ƿ�(û��modifier��������(���ֺ���ĸ)������)
		eUnkown,        // δ֪����
		eNone,          // ����
		eExist,         // ��ݼ��Ѵ���
		eConflict       // ϵͳ�ȼ�ע��ʧ��(��ռ��, ��ͻ)
	};

	enum eHotKeyType{
		eInvalidType = -1,
		// ȫ���ȼ�
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
	
	// ��ȡ��ͻ���ȼ�����
	int getConfictedHotKeyNum();
	/*
	 * ����/�����ȼ�
	 */
	void enableGlobalHotKeys(bool enable = true);
	bool isEnableGlobalHotKeys(){return m_enble;};
	/*
	 * ����ȼ������Ƿ��Ѿ�ע��
	 */
	bool isHotKeyTypeExist(eHotKeyType type);
	/*
	 * ����ȼ�״̬, keysqΪ��ʱ, ���type���͵��ȼ���ǰ״̬, ��Ϊ��ʱ���type���͵��°����Ƿ��Ѵ���
	 */
	eHotKeyState checkHotKeyState(eHotKeyType type, const QKeySequence& keysq=QKeySequence(0));
	/*
	 * ע���ݼ�, ȫ������(type<eGloableHotKeyCount)���Զ�ע��ϵͳ�ȼ�, ��ȫ�����ͽ��洢
	 * e.g. QKeySequence(Qt::ControlModifier | Qt::ShiftModifier | Qt::Key_O)
	 */
	eHotKeyState registerHotKey(eHotKeyType type, const QKeySequence& keysq);
	/*
	 * ��ע���ݼ�, ȫ������(type<eGloableHotKeyCount)���Զ���ע��ϵͳ�ȼ�, ��ȫ�����ͽ��洢
	 */
	bool unRegisterHotKey(eHotKeyType type);
	/*
	 *  ͨ�� QKeySequence �õ� Key �� modifiers
	 */
	void transKeySeq2KeyMods( const QKeySequence &keysq, Qt::Key &key, Qt::KeyboardModifiers &mods );
	/*
	 * �������ͻ�ȡ�����ַ���, δע���򷵻� "NULL"
	 * forToolTip == true ʱ, ������ע����״̬Ϊ eNoneʱ�����ַ���, ����Ϊ��
	 */
	QString getHotKeyStringByType(eHotKeyType type, bool forToolTip = false);
	/*
	 * ��ȡ�����ַ���
	 */
	QString getHotKeyStringBySq(const QKeySequence& keysq);
	/*
	 *  ͨ�� WM_HOTKEY lparam(���� Key �� Mods)��ȡ�ȼ�����, �������ڷ��� eInvalidType
	 */
	eHotKeyType getHotKeyTypeByNativeParam( long lParam );
	/*
	 * ͨ��������ȡ���� �����ڷ��� eInvalidType
	 */ 
    eHotKeyType getHotKeyTypeBySq(const QKeySequence& keysq);
	/*
	 *  ͨ�����ͻ�ȡ���� �����ڷ��� false
	 */ 
	bool getHotKeySeq(eHotKeyType type, QKeySequence& keysq);
	/*
	 *  ͨ�����ͻ�ȡ���� �����ڻ�״̬��ΪeNone���� QKeySequence()
	 */ 
	QKeySequence getHotKeySeq(eHotKeyType type);
	/*
	 * �ָ�Ĭ�ϰ���, ���Ĭ�ϰ����г�ͻ����false, ȫ��ok����true
	 */
	bool defaultHotKeys();
    /*
	 * �������ǺϷ�(�Ƿ�����Ĭ��ֵ)
	 */
    bool isHotKeyTypeValid(eHotKeyType type){return m_cacheDefaultHotkeys.contains(type);};
protected:
	hotkey_mgr();
	~hotkey_mgr();
	friend class Singleton<hotkey_mgr>;
protected:
	// ��ʼ��Ĭ�ϰ���, ���������ļ���ע��ϵͳ�ȼ�
	bool init();
	bool loadConfig();
	void initDefaultHotKeys(void);
	qint32 nativeModifiers(Qt::KeyboardModifiers modifiers);
	qint32 nativeKeycode(Qt::Key key);

	bool getNativeKeyAndMods( eHotKeyType type, quint32 &nativeKey, quint32 &nativeMods );
	bool getNativeKeyAndModsBySq( const QKeySequence &keysq, quint32 &nativeKey, quint32 &nativeMods );

	// ���С��������+shiftת��, ��ֹ�����ȼ�ע��ɹ������޷����� WM_HOTKEY
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
