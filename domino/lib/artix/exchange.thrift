#!/usr/bin/thrift -php -gen cpp

/**
 */

namespace cpp Exchange
php_namespace Exchange

enum UploadState {
	upsNone = 1,
	upsInProgress = 2,
	upsSuccess = 3,
	upsError = 4
}

struct Alert {
	1:bool isSet,
	2:string message
}

service Info {

	void setUploadState(1:UploadState state),
	UploadState getUploadState(),
	void setUploadProgress(1:i32 percent),
	i32 getUploadProgress(),
	void setUploadErrorDescription(list<string> descr),
	list<string> getUploadErrorDescription(),
	
	void setModuleDescription(1:string name, 2:list<string> descr),
	void addModuleDescription(1:string name, 2:list<string> descr),
	list<string> getModuleDescription(1:string name),
	list<string> getAvailableModules(),

	Alert getAlert(),
	void setAlert(1:string message),
	void resetAlert(),

	void startUpload(),
	void successUpload(),
	void failUpload(list<string> error)
	
}
