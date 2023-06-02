/**************************************************************************/
/*  THMHelper.h                                                           */
/**************************************************************************/
/*                       THMaker for GODOT ENGINE                         */
/*                             by BigEgg-H                                */
/**************************************************************************/
/*THM模块的基本头文件,定义了THM所有的通用的基础类型.                      */
/*请不要随意更改其中的代码,除非清除你在做什么.                            */
/**************************************************************************/
#ifndef THMHELPER_H
#define THMHELPER_H

#define SCCERR String
#define SCCERR_OK ""
#define SCCERR_FAIL "Failed"
#define SCCERR_UNKNOWN "Unknown"

/// <summary>
/// THM用户自定义数据通用类型.
/// 等价于void*
/// </summary>
typedef void* THMHUserData;

/// <summary>
/// 描述错误等级
/// </summary>
typedef enum class _THMHErrLevel {
	/// <summary>
	/// 未知错误
	/// </summary>
	Unknown = -1,
	/// <summary>
	/// 操作成功
	/// </summary>
	Success = 0,
	/// <summary>
	/// 错误
	/// </summary>
	Error = 1,
	/// <summary>
	/// 警告
	/// </summary>
	Warning,
	/// <summary>
	/// 消息
	/// </summary>
	Info,
	/// <summary>
	/// 异常
	/// </summary>
	Unexpected
}THMHErrLevel;

/// <summary>
/// 通用内部错误结构.
/// 该结构不会自动释放ErrorData,请注意清理内存.
/// </summary>
typedef struct _THMHErr {
	int ErrorCode;
	int AdditionCode;
	THMHErrLevel ErrorLevel;
	THMHUserData ErrorData;
	String ErrorInfo;
	String ErrorAdditionInfo;
	operator bool () {
		return ErrorCode == 0;
	}
	_THMHErr() {
		ErrorCode = 0;
		AdditionCode = 0;
		ErrorLevel = THMHErrLevel::Success;
		ErrorData = nullptr;
		ErrorInfo = "";
		ErrorAdditionInfo = "";
	}
	_THMHErr(const int &_ErrorCode,
			 const int &_AdditionCode,
			 const String &_ErrorInfo,
			 const String &_ErrorAdditionInfo = "",
			 const THMHErrLevel &_ErrorLevel = THMHErrLevel::Error,
			 const THMHUserData &_ErrorData = nullptr) :
			ErrorCode(_ErrorCode), AdditionCode(_AdditionCode), ErrorLevel(_ErrorLevel),
			ErrorData(_ErrorData),ErrorInfo(_ErrorInfo), ErrorAdditionInfo(_ErrorAdditionInfo) {}
	_THMHErr(const _THMHErr& father) {
		ErrorCode = father.ErrorCode;
		AdditionCode = father.AdditionCode;
		ErrorLevel = father.ErrorLevel;
		ErrorData = father.ErrorData;
		ErrorInfo = father.ErrorInfo;
		ErrorAdditionInfo = father.ErrorAdditionInfo;
	}
} THMHErr, GASErr, BPSErr;

const THMHErr *const THMHErr_Success = new THMHErr();
const THMHErr *const THMHErr_Failed = new THMHErr(1, 0, "Error");
const THMHErr *const THMHErr_Unknow = new THMHErr(1, 0, "Unknow", "", THMHErrLevel::Unknown);

_FORCE_INLINE_ THMHErr *THMH_NewErr_Success(const String &_ErrorInfo, const String &_ErrorAdditionInfo = "") {
	THMHErr *err = new THMHErr(*THMHErr_Success);
	err->ErrorInfo = _ErrorInfo;
	err->ErrorAdditionInfo = _ErrorAdditionInfo;
	return err;
}

_FORCE_INLINE_ THMHErr *THMH_NewErr_Failed(
		const int &_ErrorCode,
		const int &_AdditionCode,
		const String &_ErrorInfo,
		const String &_ErrorAdditionInfo = "",
		const THMHUserData &_ErrorData = nullptr) {
	THMHErr *err = new THMHErr(*THMHErr_Failed);
	err->ErrorCode = _ErrorCode;
	err->AdditionCode = _AdditionCode;
	err->ErrorData = _ErrorData;
	err->ErrorInfo = _ErrorInfo;
	err->ErrorAdditionInfo = _ErrorAdditionInfo;
	return err;
}

class THMHStdSysInterface {
	virtual void Initialization() = 0;
	virtual void Kill() = 0;
	virtual THMHStdSysInterface *GetSingleton() = 0;

protected:
	THMHStdSysInterface() {}
};

#define THMH_PRINT_THMHERR(pe) print(e->ErrorInfo)

#endif // !THMHELPER_H
