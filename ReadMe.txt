Usage for GUI:
	1, write remote and local port number in GUI.
	2, connect button to connect server.
	3, check in: 
		get server file list
		select the file for dependency relationship in above list
		local folder button to get local file list
		select files button to select file to check in(upload).    (Store in ./Repository/PackageName/Version/filename).
	4, check out: 
		Remote list to get server files list
	    Select files in list to download
		Download without dep button will download the select files. (Store in ./client/DownLoad/).
		Download with dep has not been finished.

Requirements:

	client2 is an automated unit test suite, it create msgConnect to connect to server, uploadmsg, uploadmsg2 to upload
	files; dependencyMsg is to upload file with dependency Xml file. downloadMsg is to download a file.
	I cannot start this package use run.bat, but I can start it using Visual Studio.

	3, package checkin is used. Like dependencyMsg could create a metadata(xml file) with dependency relationship. The 
	   meta data contains pathes of dependency files.

	4, 

	5, I store file in server directory ./Repository/PackageName/version/(files). So the file path is unique. code file and 
	   meta data is stored at this unique directory. the xml file contain all the dependency files' path.

	6, Package xml has a close property. If its value is false, the code file can be changed. But if the value is ture, files
	   in it can not be changed. If the value is true, check in process will create a new version for this package.

	7, extraction process can download specified files, but I can not download its dependency files.

	8, Sender and Receiver oneway mode channel is built to passing message for client and server communication.

	9, Client2 package is provided to demonstrate synchronous request/response or asynchronous one-way messaging using
	   http style messages.

	10, Client2 package demonstrates that channel could sending and receiving stream bytes. Client2 upload a txt and a 
		img file.

	11, Client2 is a console an automated unit test suite, it provides demo of transmission of files, and 
		Dependency -  dependencyMsg build dependency relationship (GUI can also do this).
		Close - In test2 package version 1, close is true, so check in will build a new version 2, to store files.