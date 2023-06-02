#include "modules/THMaker/TIGAS/TIGAS.h"
#include "modules/THMaker/HDVDBPS/HDVDBPS.h"
#include "GGSCCS.h"

//
//SCCFunction
//

//
//SCC
void SCCFThrowInfo(Array para) {
	GGSCCSSSingleton->LogAndPrintInfo(para[0]);
}

void SCCFThrowError(Array para) {
	GGSCCSSSingleton->LogAndPrintError(para[0]);
}

void SCCFThrowWarning(Array para) {
	GGSCCSSSingleton->LogAndPrintWarning(para[0]);
}

void SCCFInterruptProcess(Array para) {
	GGSCCSSSingleton->Kill();
}

void SCCFTravelToMap(Array para) {
	Node3D *node = Object::cast_to<Node3D>(TIGAS::GetMapMod(para[0]));
	if (node == nullptr) {
		GGSCCSSSingleton->LogAndPrintError("Map loaded failed!");
	}
	//todo:load map
}

void SCCFTravelToMainMenu(Array para) {
	//todo
}

//
//TIGAS
void SCCFLoadModPack(Array para) {
	String error = TIGAS::LoadModPack(para[0]);
	if (error != GAS_ERR_OK) {
		GGSCCSSSingleton->LogAndPrintError(error);
	}
}

void SCCFLoadModMap(Array para) {
	String error = TIGAS::LoadMapMod(para[0]);
	if (error != GAS_ERR_OK) {
		GGSCCSSSingleton->LogAndPrintError(error);
	}
}

void SCCFLoadModBullet(Array para) {
	String error = TIGAS::LoadBulletMod(para[0]);
	if (error != GAS_ERR_OK) {
		GGSCCSSSingleton->LogAndPrintError(error);
	}
}

void SCCFClearModAll(Array para) {
	String error = TIGAS::ClearModAll();
	if (error != GAS_ERR_OK) {
		GGSCCSSSingleton->LogAndPrintError(error);
	}
}

void SCCFClearModMapAll(Array para) {
	String error = TIGAS::ClearMapModAll();
	if (error != GAS_ERR_OK) {
		GGSCCSSSingleton->LogAndPrintError(error);
	}
}

void SCCFClearModBulletAll(Array para) {
	String error = TIGAS::ClearBulletModAll();
	if (error != GAS_ERR_OK) {
		GGSCCSSSingleton->LogAndPrintError(error);
	}
}

//
//HDVDBPS
void SCCFSpawnBullet(Array para) {
	HDVDBPS::SpawnBullet(para[0], para[1], para[2], para[3]);
}

void SCCFClearAllBullet(Array para) {
	HDVDBPS::ClearAllBullet();
}

//
//GGSCCS
//
void GGSCCS::_bind_methods() {
	ClassDB::bind_method(D_METHOD("SCExecute", "SC"), &GGSCCS::SCExecute);
	ClassDB::bind_method(D_METHOD("SCPush", "SC"), &GGSCCS::SCPush);
	ClassDB::bind_method(D_METHOD("SCPushAndWait", "SC"), &GGSCCS::SCPushAndWait);
	ClassDB::bind_method(D_METHOD("SCPushToLoop", "SC"), &GGSCCS::SCPushToLoop);
	ClassDB::bind_method(D_METHOD("GetSCCountTotal"), &GGSCCS::GetSCCountTotal);
	ClassDB::bind_method(D_METHOD("GetSCCountRest"), &GGSCCS::GetSCCountRest);
	ClassDB::bind_method(D_METHOD("Wait"), &GGSCCS::Wait);
	ClassDB::bind_method(D_METHOD("WaitAndStop"), &GGSCCS::WaitAndStop);
	ClassDB::bind_method(D_METHOD("WaitAndRestart"), &GGSCCS::WaitAndRestart);
	ClassDB::bind_method(D_METHOD("Stop"), &GGSCCS::Stop);
	ClassDB::bind_method(D_METHOD("Restart"), &GGSCCS::Restart);
	ClassDB::bind_method(D_METHOD("Reset"), &GGSCCS::Reset);
	ClassDB::bind_method(D_METHOD("Kill"), &GGSCCS::Kill);
	ClassDB::bind_method(D_METHOD("Continue"), &GGSCCS::Continue);
	ClassDB::bind_method(D_METHOD("ThrowError", "ErrorCode"), &GGSCCS::ThrowError);
	ClassDB::bind_method(D_METHOD("GetStateCode"), &GGSCCS::GetStateCode);
	ClassDB::bind_method(D_METHOD("GetLastErrorCode"), &GGSCCS::GetLastErrorCode);
	ClassDB::bind_method(D_METHOD("Print", "Content"), &GGSCCS::Print);
	ClassDB::bind_method(D_METHOD("PrintError", "Content"), &GGSCCS::PrintError);
	ClassDB::bind_method(D_METHOD("PrintWarning", "Content"), &GGSCCS::PrintWarning);
	ClassDB::bind_method(D_METHOD("LogInfo", "Content"), &GGSCCS::LogInfo);
	ClassDB::bind_method(D_METHOD("LogError", "Content"), &GGSCCS::LogError);
	ClassDB::bind_method(D_METHOD("LogWarning", "Content"), &GGSCCS::LogWarning);
	ClassDB::bind_method(D_METHOD("LogAndPrintInfo", "Content"), &GGSCCS::LogAndPrintInfo);
	ClassDB::bind_method(D_METHOD("LogAndPrintError", "Content"), &GGSCCS::LogAndPrintError);
	ClassDB::bind_method(D_METHOD("LogAndPrintWarning", "Content"), &GGSCCS::LogAndPrintWarning);
}

GGSCCS::GGSCCS() {
	if (GGSCCSSSingleton) {
		ERR_PRINT("THM(GGSCCS):Try to init GGSCCS again!");
		return;
	}
	BPSError err = HDVDBPS::StartSystem(BPSMode::Balance);
	if (err != BPS_ERR_OK) {
		ERR_PRINT("THM(HDVDBPS):" + err);
	}
	State = SCCState::Running;
	SCCThread = memnew(Thread);
	SCCMutex = memnew(Mutex);
	SCCThread->start(GGSCCS::SCCLoop, this);
}

void GGSCCS::SCCLoop(void *p_udata) {
	GGSCCS *ggsccs = reinterpret_cast<GGSCCS*>(p_udata);
	while (ggsccs->State != GGSCCS::SCCState::Dead) {
		while (ggsccs->State == GGSCCS::SCCState::Running) {
			ggsccs->SCCMutex->lock();
			int nc = ggsccs->LoopFuncs.size() + ggsccs->PushedFuncs.size();
			ggsccs->SCHadRest = 0;
			for (int i = 0; i < ggsccs->LoopFuncs.size(); i++) {
				ggsccs->LoopFuncs[i](ggsccs->LoopFuncParameters[i]);
				ggsccs->SCHadRest++;
			}
			if (ggsccs->NeedRestart) {
				ggsccs->LoopFuncs.clear();
				ggsccs->LoopFuncParameters.clear();
			}
			ggsccs->SCCMutex->unlock();
			ggsccs->SCCMutex->lock();
			for (int i = 0; i < ggsccs->PushedFuncs.size(); i++) {
				ggsccs->PushedFuncs[i](ggsccs->PushedFuncParameters[i]);
				ggsccs->PushedFuncs.pop_front();
				ggsccs->PushedFuncParameters.pop_front();
				ggsccs->SCHadRest++;
			}
			if (ggsccs->NeedRestart) {
				ggsccs->PushedFuncs.clear();
				ggsccs->PushedFuncParameters.clear();
			}
			ggsccs->SCHadRest = nc - ggsccs->SCHadRest;
			ggsccs->NeedRestart = false;
			ggsccs->SCCMutex->unlock();
			ggsccs->LoopTimes++;
		}
	}
}

void GGSCCS::SCExecute(Ref<SCAnalyser> SC) {
	if (!SC.ptr()->GetState()) {
		ERR_PRINT("THM(GGSCCS):Try to execute a SC without analyse!");
		return;
	}
	List<SCCFunction> funcs = SC.ptr()->FunctionsList;
	List<Array> paras = SC.ptr()->ParameterList;
	for (int i = 0; i < SC.ptr()->GetSCCount(); i++) {
		funcs[i](paras[i]);
	}
}

void GGSCCS::SCPush(Ref<SCAnalyser> SC) {
	if (!SC.ptr()->GetState()) {
		ERR_PRINT("THM(GGSCCS):Try to execute a SC without analyse!");
		return;
	}
	List<SCCFunction> funcs = SC.ptr()->FunctionsList;
	List<Array> paras = SC.ptr()->ParameterList;
	SCCMutex->lock();
	for (int i = 0; i < SC.ptr()->GetSCCount(); i++) {
		PushedFuncs.push_back(funcs[i]);
		PushedFuncParameters.push_back(paras[i]);
	}
	SCCMutex->unlock();
}

void GGSCCS::SCPushAndWait(Ref<SCAnalyser> SC) {
	SCPush(SC);
	Wait();
}

void GGSCCS::SCPushToLoop(Ref<SCAnalyser> SC) {
	if (!SC.ptr()->GetState()) {
		ERR_PRINT("THM(GGSCCS):Try to execute a SC without analyse!");
		return;
	}
	List<SCCFunction> funcs = SC.ptr()->FunctionsList;
	List<Array> paras = SC.ptr()->ParameterList;
	SCCMutex->lock();
	for (int i = 0; i < SC.ptr()->GetSCCount(); i++) {
		LoopFuncs.push_back(funcs[i]);
		LoopFuncParameters.push_back(paras[i]);
	}
	SCCMutex->unlock();
}

int GGSCCS::GetSCCountTotal() {
	return LoopFuncs.size() + PushedFuncs.size();
}

int GGSCCS::GetSCCountRest() {
	return (LoopFuncs.size() + PushedFuncs.size()) - SCHadRest;
}

void GGSCCS::Wait() {
	unsigned long long int LastTime = LoopTimes;
	while (LastTime != LoopTimes) {
	}
}

void GGSCCS::WaitAndStop() {
	unsigned long long int LastTime = LoopTimes;
	while (LastTime != LoopTimes) {
	}
	Stop();
}

void GGSCCS::WaitAndRestart() {
	Wait();
	Restart();
}

void GGSCCS::Stop() {
	if (State == SCCState::Running) {
		State = SCCState::Stop;
	}
}

void GGSCCS::Restart() {
	SCCMutex->lock();
	NeedRestart = true;
	SCCMutex->unlock();
}

void GGSCCS::Reset() {
	Restart();
}

void GGSCCS::Kill() {
	State = SCCState::Dead;
	SCCThread->wait_to_finish();
	HDVDBPS::Kill();
	TIGAS::Kill();
}

void GGSCCS::Continue() {
	if (State == SCCState::Stop || State == SCCState::Error) {
		State = SCCState::Running;
	}
}

void GGSCCS::ThrowError(int ErrorCode) {
	this->ErrorCodes.push_back(ErrorCode);
	this->LogAndPrintError("THM(GGSCCS):One error Throwing!");
	State = SCCState::Error;
}

int GGSCCS::GetStateCode() {
	switch (State) {
		case GGSCCS::SCCState::Running:
			return 0;
		case GGSCCS::SCCState::Stop:
			return 1;
		case GGSCCS::SCCState::Error:
			return 2;
		case GGSCCS::SCCState::Dead:
			return 3;
		default:
			return -1;
	}
}

int GGSCCS::GetLastErrorCode() {
	if (ErrorCodes.size() == 0) {
		return 0;
	}
	int code = ErrorCodes[ErrorCodes.size() - 1];
	ErrorCodes.pop_back();
	return code;
}

void GGSCCS::Print(const String &Content) {
	print_line(Content);
}

void GGSCCS::PrintError(const String &Content) {
	ERR_PRINT(Content);
}

void GGSCCS::PrintWarning(const String &Content) {
	WARN_PRINT(Content);
}

void GGSCCS::LogInfo(const String &Content) {
}

void GGSCCS::LogError(const String &Content) {
}

void GGSCCS::LogWarning(const String &Content) {
}

void GGSCCS::LogAndPrintInfo(const String &Content) {
	LogInfo(Time::get_singleton()->get_datetime_string_from_system() + String(" THMInfo(GGSCCS): ") + Content);
	Print(String("THMInfo(GGSCCS): ") + Content);
}

void GGSCCS::LogAndPrintError(const String &Content) {
	LogError(Time::get_singleton()->get_datetime_string_from_system() + String(" THMError(GGSCCS): ") + Content);
	PrintError(String("THMError(GGSCCS): ") + Content);
}

void GGSCCS::LogAndPrintWarning(const String &Content) {
	LogWarning(Time::get_singleton()->get_datetime_string_from_system() + String(" THMWarn(GGSCCS): ") + Content);
	PrintWarning(String("THMWarn(GGSCCS): ") + Content);
}

//
//SCAnalyser
//
void SCAnalyser::_bind_methods() {
	ClassDB::bind_method(D_METHOD("Reset"), &SCAnalyser::Reset);
	ClassDB::bind_method(D_METHOD("GetSCCount"), &SCAnalyser::GetSCCount);
	ClassDB::bind_method(D_METHOD("Analyse"), &SCAnalyser::Analyse);
	ClassDB::bind_method(D_METHOD("GASLoadModPack", "Path"), &SCAnalyser::GASLoadModPack);
	ClassDB::bind_method(D_METHOD("GASLoadModMap", "Path"), &SCAnalyser::GASLoadModMap);
	ClassDB::bind_method(D_METHOD("GASLoadModBullet", "Path"), &SCAnalyser::GASLoadModBullet);
	ClassDB::bind_method(D_METHOD("GASClearModAll"), &SCAnalyser::GASClearModAll);
	ClassDB::bind_method(D_METHOD("GASClearModMapAll"), &SCAnalyser::GASClearModMapAll);
	ClassDB::bind_method(D_METHOD("GASClearModBulletAll"), &SCAnalyser::GASClearModBulletAll);
	ClassDB::bind_method(D_METHOD("BPSSpawnBullet", "Mod", "Pos", "Dir", "TeamID"), &SCAnalyser::BPSSpawnBullet);
	ClassDB::bind_method(D_METHOD("BPSClearAll"), &SCAnalyser::BPSClearAll);
	ClassDB::bind_method(D_METHOD("GGTravelToMap", "Mod"), &SCAnalyser::GGTravelToMap);
	ClassDB::bind_method(D_METHOD("GGTravelToMainMenu"), &SCAnalyser::GGTravelToMainMenu);
	ClassDB::bind_method(D_METHOD("GGThrowInfo", "Content"), &SCAnalyser::GGThrowInfo);
	ClassDB::bind_method(D_METHOD("GGThrowError", "Content"), &SCAnalyser::GGThrowError);
	ClassDB::bind_method(D_METHOD("GGThrowWarning", "Content"), &SCAnalyser::GGThrowWarning);
	ClassDB::bind_method(D_METHOD("GGInterruptProcess", "ExitCode"), &SCAnalyser::GGInterruptProcess);
}

void SCAnalyser::Reset() {
	IsOK = false;
	FunctionsList.clear();
	ParameterList.clear();
}

int SCAnalyser::GetSCCount() {
	return FunctionsList.size();
}

void SCAnalyser::Analyse() {
	IsOK = true;
}

void SCAnalyser::GASLoadModPack(const String &Path) {
	if (IsOK) {
		ERR_PRINT("THM(GGSCCS):Try to change SC after analysing!");
		return;
	}
	Array para;
	para.append(Path);
	ParameterList.push_back(para);
	FunctionsList.push_back(SCCFLoadModPack);
}

void SCAnalyser::GASLoadModMap(const String &Path) {
	if (IsOK) {
		ERR_PRINT("THM(GGSCCS):Try to change SC after analysing!");
		return;
	}
	Array para;
	para.append(Path);
	ParameterList.push_back(para);
	FunctionsList.push_back(SCCFLoadModMap);
}

void SCAnalyser::GASLoadModBullet(const String &Path) {
	if (IsOK) {
		ERR_PRINT("THM(GGSCCS):Try to change SC after analysing!");
		return;
	}
	Array para;
	para.append(Path);
	ParameterList.push_back(para);
	FunctionsList.push_back(SCCFLoadModBullet);
}

void SCAnalyser::GASClearModAll() {
	if (IsOK) {
		ERR_PRINT("THM(GGSCCS):Try to change SC after analysing!");
		return;
	}
	Array para;
	ParameterList.push_back(para);
	FunctionsList.push_back(SCCFClearModAll);
}

void SCAnalyser::GASClearModMapAll() {
	if (IsOK) {
		ERR_PRINT("THM(GGSCCS):Try to change SC after analysing!");
		return;
	}
	Array para;
	ParameterList.push_back(para);
	FunctionsList.push_back(SCCFClearModMapAll);
}

void SCAnalyser::GASClearModBulletAll() {
	if (IsOK) {
		ERR_PRINT("THM(GGSCCS):Try to change SC after analysing!");
		return;
	}
	Array para;
	ParameterList.push_back(para);
	FunctionsList.push_back(SCCFClearModBulletAll);
}

void SCAnalyser::BPSSpawnBullet(int Mod, Vector2 Pos, Vector2 Dir, int TeamID) {
	if (IsOK) {
		ERR_PRINT("THM(GGSCCS):Try to change SC after analysing!");
		return;
	}
	Array para;
	para.append(Mod);
	para.append(Pos);
	para.append(Dir);
	para.append(TeamID);
	ParameterList.push_back(para);
	FunctionsList.push_back(SCCFSpawnBullet);
}

void SCAnalyser::BPSClearAll() {
	if (IsOK) {
		ERR_PRINT("THM(GGSCCS):Try to change SC after analysing!");
		return;
	}
	Array para;
	ParameterList.push_back(para);
	FunctionsList.push_back(SCCFClearAllBullet);
}

void SCAnalyser::GGTravelToMap(int Mod) {
	if (IsOK) {
		ERR_PRINT("THM(GGSCCS):Try to change SC after analysing!");
		return;
	}
	Array para;
	para.append(Mod);
	ParameterList.push_back(para);
	FunctionsList.push_back(SCCFTravelToMap);
}

void SCAnalyser::GGTravelToMainMenu() {
	if (IsOK) {
		ERR_PRINT("THM(GGSCCS):Try to change SC after analysing!");
		return;
	}
	Array para;
	ParameterList.push_back(para);
	FunctionsList.push_back(SCCFTravelToMainMenu);
}

void SCAnalyser::GGThrowInfo(const String &Content) {
	if (IsOK) {
		ERR_PRINT("THM(GGSCCS):Try to change SC after analysing!");
		return;
	}
	Array para;
	para.append(Content);
	ParameterList.push_back(para);
	FunctionsList.push_back(SCCFThrowInfo);
}

void SCAnalyser::GGThrowError(const String &Content) {
	if (IsOK) {
		ERR_PRINT("THM(GGSCCS):Try to change SC after analysing!");
		return;
	}
	Array para;
	para.append(Content);
	ParameterList.push_back(para);
	FunctionsList.push_back(SCCFThrowError);
}

void SCAnalyser::GGThrowWarning(const String &Content) {
	if (IsOK) {
		ERR_PRINT("THM(GGSCCS):Try to change SC after analysing!");
		return;
	}
	Array para;
	para.append(Content);
	ParameterList.push_back(para);
	FunctionsList.push_back(SCCFThrowWarning);
}

void SCAnalyser::GGInterruptProcess(int ExitCode) {
	if (IsOK) {
		ERR_PRINT("THM(GGSCCS):Try to change SC after analysing!");
		return;
	}
	Array para;
	para.append(ExitCode);
	ParameterList.push_back(para);
	FunctionsList.push_back(SCCFInterruptProcess);
}

bool SCAnalyser::GetState() {
	return IsOK;
}
