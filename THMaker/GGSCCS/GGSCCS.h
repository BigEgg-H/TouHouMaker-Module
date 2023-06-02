#ifndef GGSCCS_H
#define GGSCCS_H

#include "core/object/class_db.h"
#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "core/variant/variant.h"
#include "core/math/vector2.h"
#include "core/templates/list.h"
#include "core/os/thread.h"
#include "core/os/mutex.h"
#include "core/os/time.h"
#include "core/io/resource.h"

typedef void (*SCCFunction)(Array Parameter);

/// <summary>
/// SCAnalyser
/// SC分析器
/// </summary>
class SCAnalyser : public Resource {
	GDCLASS(SCAnalyser, Resource);

protected:
	static void _bind_methods();

public:
	void Reset();
	int  GetSCCount();
	void Analyse();
	void GASLoadModPack(const String &Path);
	void GASLoadModMap(const String &Path);
	void GASLoadModBullet(const String &Path);
	void GASClearModAll();
	void GASClearModMapAll();
	void GASClearModBulletAll();
	void BPSSpawnBullet(int Mod, Vector2 Pos, Vector2 Dir, int TeamID);
	void BPSClearAll();
	void GGTravelToMap(int Mod);
	void GGTravelToMainMenu();
	void GGThrowInfo(const String &Content);
	void GGThrowError(const String &Content);
	void GGThrowWarning(const String &Content);
	void GGInterruptProcess(int ExitCode);

public:
	SCAnalyser() {}
	SCAnalyser(const SCAnalyser& Mother) {
		this->FunctionsList = Mother.FunctionsList;
		this->ParameterList = Mother.ParameterList;
	}
	bool GetState();
	List<SCCFunction> FunctionsList;
	List<Array> ParameterList;

private:
	bool IsOK = false;

};

/// <summary>
/// GGSCCS
/// 全局游戏流式命令控制系统
/// </summary>
class GGSCCS : public Object {
	GDCLASS(GGSCCS, Object);

protected:
	static void _bind_methods();

private:
	enum class SCCState {
		Running,Stop,Error,Dead
	};
	SCCState State = SCCState::Dead;
	List<int> ErrorCodes;
	Thread* SCCThread;
	Mutex* SCCMutex;
	List<SCCFunction> LoopFuncs;
	List<Array> LoopFuncParameters;
	List<SCCFunction> PushedFuncs;
	List<Array> PushedFuncParameters;
	unsigned long long int LoopTimes = 0;
	int SCHadRest = 0;
	bool NeedRestart = false;

public:
	GGSCCS();
	static void SCCLoop(void *p_udata);

public:
	void SCExecute(Ref<SCAnalyser> SC);
	void SCPush(Ref<SCAnalyser> SC);
	void SCPushAndWait(Ref<SCAnalyser> SC);
	void SCPushToLoop(Ref<SCAnalyser> SC);
	int  GetSCCountTotal();
	int  GetSCCountRest();
	void Wait();
	void WaitAndStop();
	void WaitAndRestart();
	void Stop();
	void Restart();
	void Reset();
	void Kill();
	void Continue();
	void ThrowError(int ErrorCode);
	int  GetStateCode();
	int  GetLastErrorCode();
	void Print(const String& Content);
	void PrintError(const String& Content);
	void PrintWarning(const String& Content);
	void LogInfo(const String& Content);
	void LogError(const String& Content);
	void LogWarning(const String& Content);
	void LogAndPrintInfo(const String& Content);
	void LogAndPrintError(const String& Content);
	void LogAndPrintWarning(const String& Content);
};
static GGSCCS *GGSCCSSSingleton = NULL;

#endif // GGSCCS_H
