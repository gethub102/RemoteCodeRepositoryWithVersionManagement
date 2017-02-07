/////////////////////////////////////////////////////////////////////
// Window.cpp - C++\CLI implementation of WPF Application          //
//            - Demo for CSE 687 Project #4                        //
//  ver 3.1                                                        //
//  Language:      Visual C++ 2015, SP1                            //
//  Platform:      Macbook, Win10                                  //
//  Author:        Wenbin Li,      Syracuse University             //
//                 wli102@syr.edu                                  //
//              Source from Dr. Jim                                //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package demonstrates how to build a C++\CLI WPF application.  It
*  provides one class, WPFCppCliDemo, derived from System::Windows::Window
*  that is compiled with the /clr option to run in the Common Language
*  Runtime, and another class MockChannel written in native C++ and compiled
*  as a DLL with no Common Language Runtime support.
*
*  The window class hosts, in its window, a tab control with three views, two
*  of which are provided with functionality that you may need for Project #4.
*  It loads the DLL holding MockChannel.  MockChannel hosts a send queue, a
*  receive queue, and a C++11 thread that reads from the send queue and writes
*  the deQ'd message to the receive queue.
*
*  The Client can post a message to the MockChannel's send queue.  It hosts
*  a receive thread that reads the receive queue and dispatches any message
*  read to a ListBox in the Client's FileList tab.  So this Demo simulates
*  operations you will need to execute for Project #4.
*
*  Required Files:
*  ---------------
*  Window.h, Window.cpp, MochChannel.h, MochChannel.cpp,
*  BlockingQueue.h, BlockingQueue.cpp, Client.h, Client.cpp
*  HttpMessage.cpp, HttpMessage.h, FileSystem.h, FileSystem.cpp
*  Sender.cpp, Sender.h, Receiver.cpp, Receiver.h, XmlDocument.h
*  XmlDocument.cpp, XmlElement.cpp, XmlElement.h, XmlParser.cpp
*  XmlParser.h, XmlElementParts.h, XmlElementParts.cpp, Tokeniner.h
*  Tokeniner.cpp, itokcollection.h
*
*  Build Command:
*  --------------
*  devenv StringSocketServer.sln /rebuild debug
*
*  Maintenance History:
*  --------------------
*  ver 3.1 : 03 May 2016
*  - add check In view, check out view.
*  - add check in buttons & handler fuctions, such as void checkIn(..), 
*    void clearCheckInList(..)
*  - add check out buttons & handler fuctions, such as void checkOutWithOutDep(..)
*	 void checkOutWithDep(..)
*  ver 3.0 : 22 Apr 2016
*  - added support for multi selection of Listbox items.  Implemented by
*    Saurabh Patel.  Thanks Saurabh.
*  ver 2.0 : 15 Apr 2015
*  - completed message passing demo with moch channel
*  - added FileBrowserDialog to show files in a selected directory
*  ver 1.0 : 13 Apr 2015
*  - incomplete demo with GUI but not connected to mock channel
*/
#include "Window.h"
using namespace CppCliWindows;
/* construtor */
WPFCppCliDemo::WPFCppCliDemo()
{
	// set up channel
	ObjectFactory* pObjFact = new ObjectFactory;
	pSendr_ = pObjFact->createSendr();
	pRecvr_ = pObjFact->createRecvr();
	pChann_ = pObjFact->createMockChannel(pSendr_, pRecvr_);
	//pChann_->startChanThread();
	delete pObjFact;

	// client's receive thread

	recvThread = gcnew Thread(gcnew ThreadStart(this, &WPFCppCliDemo::getMessage));
	recvThread->Start();

	// set event handlers

	setEventHandler();

	// set Window properties

	this->Title = "WPF C++/CLI Demo";
	this->Width = 800;
	this->Height = 600;

	// attach dock panel to Window

	this->Content = hDockPanel;
	hDockPanel->Children->Add(hStatusBar);
	hDockPanel->SetDock(hStatusBar, Dock::Bottom);
	hDockPanel->Children->Add(hGrid);

	// setup Window controls and views

	setUpTabControl();
	setUpStatusBar();
	setUpSendMessageView();
	setUpFileListView();
	setUpConnectionView();
	setUpCheckInView();
	setUpCheckOutView();
}
/* deconstructor */
WPFCppCliDemo::~WPFCppCliDemo()
{
	delete pChann_;
	delete pSendr_;
	delete pRecvr_;
}
/* set event handler */
void CppCliWindows::WPFCppCliDemo::setEventHandler()
{

	this->Loaded +=
		gcnew System::Windows::RoutedEventHandler(this, &WPFCppCliDemo::OnLoaded);
	this->Closing +=
		gcnew CancelEventHandler(this, &WPFCppCliDemo::Unloading);
	hSendButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::sendMessage);
	hClearButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::clear);
	hFolderBrowseButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::browseForFolder);
	hShowItemsButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::getItemsFromList);
	hConnectButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::connectToServer);
	hChGetlistButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::getFileListFromServer);
	hChSelectlistButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::selectRemoteFiles);
	hChShowItemsButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::browsLocalFiles);
	hChSelectButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::selectLocalFiles);
	hChCheckInButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::checkIn);
	hChClearButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::clearCheckInList);
	hChOutGetlistButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::getRemoteFilesList);
	hChOutSelectlistButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::selectRemoteFilesForDown);
	hChOutWithOutDepsButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::checkOutWithOutDep);
	hChClearDownRetButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::clearDownloadResult);
	hChOutWithDepsButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::checkOutWithDep);
}
/* set status bar */
void WPFCppCliDemo::setUpStatusBar()
{
	hStatusBar->Items->Add(hStatusBarItem);
	hStatus->Text = "very important messages will appear here";
	//hStatus->FontWeight = FontWeights::Bold;
	hStatusBarItem->Content = hStatus;
	hStatusBar->Padding = Thickness(10, 2, 10, 2);
}
/* set the control tab */
void WPFCppCliDemo::setUpTabControl()
{
	hGrid->Children->Add(hTabControl);
	hConnectTab->Header = "Connect";
	hCheckInTab->Header = "Check In";
	hCheckOutTab->Header = "Check Out";
	hSendMessageTab->Header = "Send Message";
	hFileListTab->Header = "File List";
	
	hTabControl->Items->Add(hConnectTab);
	hTabControl->Items->Add(hCheckInTab);
	hTabControl->Items->Add(hCheckOutTab);
	hTabControl->Items->Add(hSendMessageTab);
	hTabControl->Items->Add(hFileListTab);
	
}
/* set Text Block Properties */
void WPFCppCliDemo::setTextBlockProperties()
{
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hSendMessageGrid->RowDefinitions->Add(hRow1Def);
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Child = hTextBlock1;
	hTextBlock1->Padding = Thickness(15);
	hTextBlock1->Text = "";
	hTextBlock1->FontFamily = gcnew Windows::Media::FontFamily("Tahoma");
	hTextBlock1->FontWeight = FontWeights::Bold;
	hTextBlock1->FontSize = 16;
	hScrollViewer1->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
	hScrollViewer1->Content = hBorder1;
	hSendMessageGrid->SetRow(hScrollViewer1, 0);
	hSendMessageGrid->Children->Add(hScrollViewer1);
}
/* set button properties */
void WPFCppCliDemo::setButtonsProperties()
{
	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(75);
	hSendMessageGrid->RowDefinitions->Add(hRow2Def);
	hSendButton->Content = "Send Message";
	Border^ hBorder2 = gcnew Border();
	hBorder2->Width = 120;
	hBorder2->Height = 30;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hClearButton->Content = "Clear";
	hBorder2->Child = hSendButton;
	Border^ hBorder3 = gcnew Border();
	hBorder3->Width = 120;
	hBorder3->Height = 30;
	hBorder3->BorderThickness = Thickness(1);
	hBorder3->BorderBrush = Brushes::Black;
	hBorder3->Child = hClearButton;
	hStackPanel1->Children->Add(hBorder2);
	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 10;
	hStackPanel1->Children->Add(hSpacer);
	hStackPanel1->Children->Add(hBorder3);
	hStackPanel1->Orientation = Orientation::Horizontal; // horizontal button
	hStackPanel1->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hSendMessageGrid->SetRow(hStackPanel1, 1);
	hSendMessageGrid->Children->Add(hStackPanel1);
}
/* get local url string */
std::string CppCliWindows::WPFCppCliDemo::localUrl()
{
	auto localIp = toStdString(hLocalIpTextBox->Text);
	if (localIp == "127.0.0.1")
		localIp = "localhost";
	auto localPort = toStdString(hLocalPortTextBox->Text);
	auto localurl = localIp + ":" + localPort;
	return localurl;
}
	

/* get remote url string */
std::string CppCliWindows::WPFCppCliDemo::remoteUrl()
{
	auto remoteIp = toStdString(hIpTextBox->Text);
	if (remoteIp == "127.0.0.1")
		remoteIp = "localhost";
	auto remotePort = toStdString(hPortTextBox->Text);
	auto remoteurl = remoteIp + ":" + remotePort;
	return remoteurl;
}
/* set up the send message view */
void WPFCppCliDemo::setUpSendMessageView()
{
	Console::Write("\n  setting up sendMessage view");
	hSendMessageGrid->Margin = Thickness(20);
	hSendMessageTab->Content = hSendMessageGrid;

	setTextBlockProperties();
	setButtonsProperties();
}
/* system string convert to std string */
std::string WPFCppCliDemo::toStdString(String^ pStr)
{
	std::string dst;
	for (int i = 0; i < pStr->Length; ++i)
		dst += (char)pStr[i];
	return dst;
}
/* send message function */
void WPFCppCliDemo::sendMessage(Object^ obj, RoutedEventArgs^ args)
{
	// configure the local and remote url
	
	auto localurl = localUrl();

	auto remoteurl = remoteUrl();

	auto msg = pSendr_->createMsgForCheckIn("GetList", remoteurl, localurl, "root");


	pSendr_->postMessage(msg);
	Console::Write("\n  sent message");
	hStatus->Text = "Sent message";
}
/* change std string to system string */
String^ WPFCppCliDemo::toSystemString(std::string& str)
{
	StringBuilder^ pStr = gcnew StringBuilder();
	for (size_t i = 0; i < str.size(); ++i)
		pStr->Append((Char)str[i]);
	return pStr->ToString();
}
/* update UI of connect text  */
void WPFCppCliDemo::addText(String^ msg)
{
	hTextBlock1->Text += msg + "\n\n";
	hStatusTextBox->Text = "CONNECTED";
}
/* update the UI of checkin and checkout list box */
void WPFCppCliDemo::addGetListText(String^ msg)
{
	std::vector<std::string> files;
	files = pSendr_->parserBodyList(toStdString(msg));
	hChRemoteFileListBox->Items->Clear();
	hChOutRemoteFileListBox->Items->Clear();
	for each (auto itemfile in files) {
		String^ item = toSystemString(itemfile);
		hChRemoteFileListBox->Items->Add(item);
		hChOutRemoteFileListBox->Items->Add(item);
	}
}
/* display result of downloading */
void CppCliWindows::WPFCppCliDemo::addDownResultText(String ^ msg)
{
	hChOutResultListBox->Items->Add(msg);
}

/* this is a method started by recev thread */
void WPFCppCliDemo::getMessage()
{
	// recvThread runs this function

	while (!stopRecv_)
	{
		std::cout << "\n  receive thread calling getMessage()";
		auto msg = pRecvr_->getMessage();     // recv give me msg
		auto retMsg = pChann_->parserRcMsg(msg);  // parser msg and get msg body string
		String^ sMsg = toSystemString(retMsg);
		array<String^>^ args = gcnew array<String^>(1);
		args[0] = sMsg;

		Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addText);
		Dispatcher->Invoke(act, args);  // must call addText on main UI thread

		Action<String^>^ getListAct = gcnew Action<String^>(this, &WPFCppCliDemo::addGetListText);
		Action<String^>^ downloadRetAct = gcnew Action<String^>(this, &WPFCppCliDemo::addDownResultText);
		if (msg.command() == "SERVER_ACK")
			Dispatcher->Invoke(getListAct, args);
		if (msg.command() == "DOWNLOAD_ACK") {
			auto file_name = msg.fileName();
			sMsg = toSystemString(file_name);
			args[0] = sMsg;
			Dispatcher->Invoke(downloadRetAct, args);
		}
			
	}
}
/* clear list item in list */
void WPFCppCliDemo::clear(Object^ sender, RoutedEventArgs^ args)
{
	Console::Write("\n  cleared message text");
	hStatus->Text = "Cleared message";
	hTextBlock1->Text = "";
}


/* select files in list */
void WPFCppCliDemo::getItemsFromList(Object^ sender, RoutedEventArgs^ args)
{
	int index = 0;
	int count = hListBox->SelectedItems->Count;
	hStatus->Text = "Show Selected Items";
	array<System::String^>^ items = gcnew array<String^>(count);
	if (count > 0) {
		for each (String^ item in hListBox->SelectedItems)
		{
			items[index++] = item;
		}
	}

	hListBox->Items->Clear();
	if (count > 0) {
		for each (String^ item in items)
		{
			hListBox->Items->Add(item);
		}
	}
}


/* set file list view */
void WPFCppCliDemo::setUpFileListView()
{
	Console::Write("\n  setting up FileList view");
	hFileListGrid->Margin = Thickness(20);              // set grid
	hFileListTab->Content = hFileListGrid;              // should only add one item, but it is grid can contain
	RowDefinition^ hRow1Def = gcnew RowDefinition();    // create a row
	//hRow1Def->Height = GridLength(75);
	hFileListGrid->RowDefinitions->Add(hRow1Def);       // add one row in grid
	Border^ hBorder1 = gcnew Border();                  // 
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hListBox->SelectionMode = SelectionMode::Multiple;
	hBorder1->Child = hListBox;
	hFileListGrid->SetRow(hBorder1, 0);
	hFileListGrid->Children->Add(hBorder1);             // will add the list box to the grid

	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(75);
	RowDefinition^ hRow2Def2 = gcnew RowDefinition();
	hRow2Def2->Height = GridLength(75);
	hFileListGrid->RowDefinitions->Add(hRow2Def);
	hFileListGrid->RowDefinitions->Add(hRow2Def2);     // add two new rows, but empty
	hFolderBrowseButton->Content = "Select Directory";
	hFolderBrowseButton->Height = 30;
	hFolderBrowseButton->Width = 120;
	hFolderBrowseButton->BorderThickness = Thickness(2);
	hFolderBrowseButton->BorderBrush = Brushes::Black;
	hFileListGrid->SetRow(hFolderBrowseButton, 1);
	hFileListGrid->Children->Add(hFolderBrowseButton); // add button

	// Show selected items button.
	hShowItemsButton->Content = "Show Selected Items";
	hShowItemsButton->Height = 30;
	hShowItemsButton->Width = 120;
	hShowItemsButton->BorderThickness = Thickness(2);
	hShowItemsButton->BorderBrush = Brushes::Black;
	hFileListGrid->SetRow(hShowItemsButton, 2);
	hFileListGrid->Children->Add(hShowItemsButton);    // add buttion

	hFolderBrowserDialog->ShowNewFolderButton = false;
	hFolderBrowserDialog->SelectedPath = System::IO::Directory::GetCurrentDirectory();
}


/* brow the folder */
void WPFCppCliDemo::browseForFolder(Object^ sender, RoutedEventArgs^ args)
{
	std::cout << "\n  Browsing for folder";
	hListBox->Items->Clear();
	System::Windows::Forms::DialogResult result;
	result = hFolderBrowserDialog->ShowDialog();
	if (result == System::Windows::Forms::DialogResult::OK)
	{
		String^ path = hFolderBrowserDialog->SelectedPath;
		std::cout << "\n  opening folder \"" << toStdString(path) << "\"";
		array<String^>^ files = System::IO::Directory::GetFiles(path, L"*.*");
		for (int i = 0; i < files->Length; ++i)
			hListBox->Items->Add(files[i]);
		array<String^>^ dirs = System::IO::Directory::GetDirectories(path);
		for (int i = 0; i < dirs->Length; ++i)
			hListBox->Items->Add(L"<> " + dirs[i]);
	}
}

/* call when the connect button is clicked */
void CppCliWindows::WPFCppCliDemo::connectToServer(Object ^ sender, RoutedEventArgs ^ args)
{
	Console::Write("\n Connect button is clicked");
	auto rePort = hPortTextBox->Text;
	auto reIp = hIpTextBox->Text;
	auto lcPort = hLocalPortTextBox->Text;
	auto lcIp = hLocalIpTextBox->Text;
	Console::Write("\n ip = " + lcIp + ", port = " + lcPort);

	pRecvr_->startRecvThread(toStdString(lcPort)); // how to conver system string to string 
	pSendr_->startSendThread();
	
	auto msg = pSendr_->createMsgForCheckIn("CONNECT", remoteUrl(), localUrl(), "Path");

	pSendr_->postMessage(msg);
	Console::Write("\n  sent connect information");
	hStatus->Text = "Connecting to server...";
}
/* get file list from server */
void CppCliWindows::WPFCppCliDemo::getFileListFromServer(Object ^ sender, RoutedEventArgs ^ args)
{
	Console::Write("\n  get file list from server");
	hStatus->Text = "get list from server";
	auto msg = pSendr_->createMsgForCheckIn("GetList", remoteUrl(), localUrl(), "root");
	pSendr_->postMessage(msg);
}
/* select the files from server list to create dependency list */
void CppCliWindows::WPFCppCliDemo::selectRemoteFiles(Object ^ sender, RoutedEventArgs ^ args)
{
	Console::Write("\n  select files for create dependency");
	int index = 0;
	int count = hChRemoteFileListBox->SelectedItems->Count;
	hStatus->Text = "Selected Files to create dependency";
	array<System::String^>^ items = gcnew array<String^>(count);
	if (count > 0) {
		for each (String^ item in hChRemoteFileListBox->SelectedItems)
		{
			items[index++] = item;
		}
	}

	hChRemoteFileListBox->Items->Clear();
	if (count > 0) {
		for each (String^ item in items)
		{
			hChRemoteFileListBox->Items->Add(item);
		}
	}
}
/* browse the local files */
void CppCliWindows::WPFCppCliDemo::browsLocalFiles(Object ^ sender, RoutedEventArgs ^ args)
{
	std::cout << "\n  Browsing for folder";
	hChLocalFileListBox->Items->Clear();
	System::Windows::Forms::DialogResult result;
	result = hFolderBrowserDialog->ShowDialog();
	if (result == System::Windows::Forms::DialogResult::OK)
	{
		String^ path = hFolderBrowserDialog->SelectedPath;
		std::cout << "\n  opening folder \"" << toStdString(path) << "\"";
		array<String^>^ files = System::IO::Directory::GetFiles(path, L"*.*");
		for (int i = 0; i < files->Length; ++i)
			hChLocalFileListBox->Items->Add(files[i]);
		array<String^>^ dirs = System::IO::Directory::GetDirectories(path);
		for (int i = 0; i < dirs->Length; ++i)
			hChLocalFileListBox->Items->Add(L"<> " + dirs[i]);
	}
}

/* select the package or files for checking in */
void CppCliWindows::WPFCppCliDemo::selectLocalFiles(Object ^ sender, RoutedEventArgs ^ args)
{
	Console::Write("\n  select files for checking in");
	int index = 0;
	int count = hChLocalFileListBox->SelectedItems->Count;
	hStatus->Text = "Selected Files to Check In";
	array<System::String^>^ items = gcnew array<String^>(count);
	if (count > 0) {
		for each (String^ item in hChLocalFileListBox->SelectedItems)
		{
			items[index++] = item;
		}
	}

	hChLocalFileListBox->Items->Clear();
	if (count > 0) {
		for each (String^ item in items)
		{
			hChLocalFileListBox->Items->Add(item);
		}
	}
}
/* check in process */
void CppCliWindows::WPFCppCliDemo::checkIn(Object ^ sender, RoutedEventArgs ^ args)
{
	std::string time = "2016"; // this time stamp should be in msg to create version
	Console::Write("\n  client checking in " + toSystemString(time));
	hStatus->Text = "cliked Check In";
	std::string bodyStr;
	for each (auto item in hChRemoteFileListBox->Items) {
		bodyStr = bodyStr + toStdString(item->ToString()) + "\n";
	}
	if (hChLocalFileListBox->Items->Count > 0)
	{
		for each (auto item in hChLocalFileListBox->Items) {   //upload all the files in folder list
			auto msg = pSendr_->createMsgForCheckIn("Upload", remoteUrl(), localUrl(), toStdString(item->ToString()));
			msg = pSendr_->createMsgForCheckInWithTimeStamp("Upload", remoteUrl(), localUrl(), toStdString(item->ToString()), time, bodyStr);
			pSendr_->postMessage(msg);
		}
		
	}
}
/* clear the check in view list */
void CppCliWindows::WPFCppCliDemo::clearCheckInList(Object ^ sender, RoutedEventArgs ^ args)
{
	Console::Write("\n  Clear check in list");
	hStatus->Text = "Clear check in list";
	hChLocalFileListBox->Items->Clear();
	hChRemoteFileListBox->Items->Clear();
}
/* get file list from server */
void CppCliWindows::WPFCppCliDemo::getRemoteFilesList(Object ^ sender, RoutedEventArgs ^ args)
{
	Console::Write("\n  Get file list from server");
	hStatus->Text = "Get list from server";
	auto msg = pSendr_->createMsgForCheckIn("GetList", remoteUrl(), localUrl(), "root");
	pSendr_->postMessage(msg);
}
/* select the file in list for downloading */
void CppCliWindows::WPFCppCliDemo::selectRemoteFilesForDown(Object ^ sender, RoutedEventArgs ^ args)
{
	Console::Write("\n  Select files for downloading files ");
	int index = 0;
	int count = hChOutRemoteFileListBox->SelectedItems->Count;
	hStatus->Text = "Selected Files to download files ";
	array<System::String^>^ items = gcnew array<String^>(count);
	if (count > 0) {
		for each (String^ item in hChOutRemoteFileListBox->SelectedItems)
		{
			items[index++] = item;
		}
	}

	hChOutRemoteFileListBox->Items->Clear();
	if (count > 0) {
		for each (String^ item in items)
		{
			hChOutRemoteFileListBox->Items->Add(item);
		}
	}
}

/* download files without dependency files */
void CppCliWindows::WPFCppCliDemo::checkOutWithOutDep(Object ^ sender, RoutedEventArgs ^ args)
{
	Console::Write("\n  download files without dependency files");
	hStatus->Text = "download files without dependency files";
	if (hChOutRemoteFileListBox->Items->Count > 0)
	{
		for each (auto item in hChOutRemoteFileListBox->Items) {   //upload all the files in folder list
			auto t = toStdString(item->ToString());
			auto msg = pSendr_->createMsgForCheckIn("DownLoad", remoteUrl(), localUrl(), toStdString(item->ToString()));

			pSendr_->postMessage(msg);
		}
	}
}
/* download the files from server with dependency files  */
void CppCliWindows::WPFCppCliDemo::checkOutWithDep(Object ^ sender, RoutedEventArgs ^ args)
{
	Console::Write("\n  Download with dependency files");
	hStatus->Text = "Download with dependency files";
}
/* clear download result */
void CppCliWindows::WPFCppCliDemo::clearDownloadResult(Object ^ sender, RoutedEventArgs ^ args)
{
	Console::Write("\n  cleared message text");
	hStatus->Text = "Cleared message";
	hChOutResultListBox->Items->Clear();
	hChOutRemoteFileListBox->Items->Clear();
}

/* set row and col for connect view tab */
void CppCliWindows::WPFCppCliDemo::setUpConnectionViewRowCol()
{
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	RowDefinition^ hRow2Def = gcnew RowDefinition();
	RowDefinition^ hRow3Def = gcnew RowDefinition();
	RowDefinition^ hRow4Def = gcnew RowDefinition();
	hConnectGrid->RowDefinitions->Add(hRow1Def);
	hConnectGrid->RowDefinitions->Add(hRow2Def);
	hConnectGrid->RowDefinitions->Add(hRow3Def);
	hConnectGrid->RowDefinitions->Add(hRow4Def);
	hRow1Def->Height = GridLength(75);
	hRow2Def->Height = GridLength(75);
	hRow3Def->Height = GridLength(75);
	hRow4Def->Height = GridLength(75);

	ColumnDefinition^ hCol1Def = gcnew ColumnDefinition();
	ColumnDefinition^ hCol2Def = gcnew ColumnDefinition();
	ColumnDefinition^ hCol3Def = gcnew ColumnDefinition();
	hConnectGrid->ColumnDefinitions->Add(hCol1Def);
	hConnectGrid->ColumnDefinitions->Add(hCol2Def);
	hConnectGrid->ColumnDefinitions->Add(hCol3Def);
	hCol1Def->Width = GridLength(95);
	hCol2Def->Width = GridLength(200);
	hCol3Def->Width = GridLength(200);
}
/* set up ip and port number view in connect tab */
void CppCliWindows::WPFCppCliDemo::setUpConnectIpAndPortLable()
{
	hIpLable->Content = "Remote Ip:";
	hIpLable->FontSize = 16;
	hIpLable->Width = 100;
	hIpLable->Height = 30;
	hConnectGrid->SetRow(hIpLable, 0);
	hConnectGrid->SetColumn(hIpLable, 0);
	hConnectGrid->Children->Add(hIpLable);

	hPortLable->Content = "Remote Port:";
	hPortLable->FontSize = 16;
	hPortLable->Width = 100;
	hPortLable->Height = 30;
	hConnectGrid->SetRow(hPortLable, 1);
	hConnectGrid->SetColumn(hPortLable, 0);
	hConnectGrid->Children->Add(hPortLable);

	hLocalIpLable->Content = "Local Ip:";
	hLocalIpLable->FontSize = 16;
	hLocalIpLable->Width = 100;
	hLocalIpLable->Height = 30;
	hConnectGrid->SetRow(hLocalIpLable, 2);
	hConnectGrid->SetColumn(hLocalIpLable, 0);
	hConnectGrid->Children->Add(hLocalIpLable);

	hLocalPortLable->Content = "Local Port:";
	hLocalPortLable->FontSize = 16;
	hLocalPortLable->Width = 100;
	hLocalPortLable->Height = 30;
	hConnectGrid->SetRow(hLocalPortLable, 3);
	hConnectGrid->SetColumn(hLocalPortLable, 0);
	hConnectGrid->Children->Add(hLocalPortLable);

	hIpTextBox->Text = "127.0.0.1";
	hIpTextBox->FontSize = 16;
	hIpTextBox->Width = 100;
	hIpTextBox->Height = 30;
	hConnectGrid->SetRow(hIpTextBox, 0);
	hConnectGrid->SetColumn(hIpTextBox, 1);
	hConnectGrid->Children->Add(hIpTextBox);
}
/* set connection view */
void WPFCppCliDemo::setUpConnectionView()
{
	Console::Write("\n  setting up Connection view");
	hConnectGrid->Margin = Thickness(20);   // add connect button
	hConnectTab->Content = hConnectGrid;
	setUpConnectionViewRowCol();            // set view row and col
	setUpConnectIpAndPortLable();

	hPortTextBox->Text = "8080";
	hPortTextBox->FontSize = 16;
	hPortTextBox->Width = 100;
	hPortTextBox->Height = 30;
	hConnectGrid->SetRow(hPortTextBox, 1);
	hConnectGrid->SetColumn(hPortTextBox, 1);
	hConnectGrid->Children->Add(hPortTextBox);

	hLocalIpTextBox->Text = "localhost";
	hLocalIpTextBox->FontSize = 16;
	hLocalIpTextBox->Width = 100;
	hLocalIpTextBox->Height = 30;
	hConnectGrid->SetRow(hLocalIpTextBox, 2);
	hConnectGrid->SetColumn(hLocalIpTextBox, 1);
	hConnectGrid->Children->Add(hLocalIpTextBox);

	hLocalPortTextBox->Text = "8089";
	hLocalPortTextBox->FontSize = 16;
	hLocalPortTextBox->Width = 100;
	hLocalPortTextBox->Height = 30;
	hConnectGrid->SetRow(hLocalPortTextBox, 3);
	hConnectGrid->SetColumn(hLocalPortTextBox, 1);
	hConnectGrid->Children->Add(hLocalPortTextBox);

	hStatusTextBox->Text = "UNCONNECT";
	hStatusTextBox->FontSize = 16;
	hStatusTextBox->Width = 100;
	hStatusTextBox->Height = 30;
	hConnectGrid->SetRow(hStatusTextBox, 0);
	hConnectGrid->SetColumn(hStatusTextBox, 2);
	hConnectGrid->Children->Add(hStatusTextBox);

	hConnectButton->Content = "Connect";
	hConnectButton->Height = 30;
	hConnectButton->Width = 100;
	hConnectGrid->SetRow(hConnectButton, 1);
	hConnectGrid->SetColumn(hConnectButton, 2);
	hConnectGrid->Children->Add(hConnectButton);
}

/* check tab grid row and col set up */
void CppCliWindows::WPFCppCliDemo::setUpCheckInViewRowCol()
{
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	RowDefinition^ hRow2Def = gcnew RowDefinition();
	RowDefinition^ hRow3Def = gcnew RowDefinition();
	hCheckInGrid->RowDefinitions->Add(hRow1Def);
	hCheckInGrid->RowDefinitions->Add(hRow2Def);
	hCheckInGrid->RowDefinitions->Add(hRow3Def);
	hRow1Def->Height = GridLength(370);
	hRow2Def->Height = GridLength(50);
	hRow3Def->Height = GridLength(50);

	ColumnDefinition^ hCol1Def = gcnew ColumnDefinition();
	ColumnDefinition^ hCol2Def = gcnew ColumnDefinition();
	ColumnDefinition^ hCol3Def = gcnew ColumnDefinition();
	hCheckInGrid->ColumnDefinitions->Add(hCol1Def);
	hCheckInGrid->ColumnDefinitions->Add(hCol2Def);
	hCheckInGrid->ColumnDefinitions->Add(hCol3Def);
	hCol1Def->Width = GridLength(345);
	hCol2Def->Width = GridLength(50);
	hCol3Def->Width = GridLength(345);
}
/* set up some buttons in checkin view */
void CppCliWindows::WPFCppCliDemo::setUpCheckInViewPartsButton()
{
	hChGetlistButton->Content = "GetList";
	hChGetlistButton->Height = 30;
	hChGetlistButton->Width = 100;
	hCheckInGrid->SetRow(hChGetlistButton, 1);
	hCheckInGrid->SetColumn(hChGetlistButton, 0);
	hCheckInGrid->Children->Add(hChGetlistButton);

	hChSelectlistButton->Content = "Add Deps";
	hChSelectlistButton->Height = 30;
	hChSelectlistButton->Width = 100;
	hCheckInGrid->SetRow(hChSelectlistButton, 2);
	hCheckInGrid->SetColumn(hChSelectlistButton, 0);
	hCheckInGrid->Children->Add(hChSelectlistButton);

	hChClearButton->Content = "Clear";
	hChClearButton->Height = 30;
	hChClearButton->Width = 50;
	hCheckInGrid->SetRow(hChClearButton, 1);
	hCheckInGrid->SetColumn(hChClearButton, 1);
	hCheckInGrid->Children->Add(hChClearButton);

	hChCheckInButton->Content = "CheckIn";
	hChCheckInButton->Height = 30;
	hChCheckInButton->Width = 50;
	hCheckInGrid->SetRow(hChCheckInButton, 2);
	hCheckInGrid->SetColumn(hChCheckInButton, 1);
	hCheckInGrid->Children->Add(hChCheckInButton);
}

/* set up the check in process tab item */
void CppCliWindows::WPFCppCliDemo::setUpCheckInView()
{
	Console::Write("\n  setting up CheckIn view");
	hCheckInGrid->Margin = Thickness(20);   // add connect button
	hCheckInTab->Content = hCheckInGrid;
	setUpCheckInViewRowCol();
	
	//hChLocalFileListBox->

	Border^ hBorder1 = gcnew Border();                  // 
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hChRemoteFileListBox->SelectionMode = SelectionMode::Multiple;
	hBorder1->Child = hChRemoteFileListBox;
	hCheckInGrid->SetRow(hBorder1, 0);
	hCheckInGrid->SetColumn(hBorder1, 0);
	hCheckInGrid->Children->Add(hBorder1);

	setUpCheckInViewPartsButton(); //set up parts of buttons in check in view

	Border^ hBorder2 = gcnew Border();                  // 
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hChLocalFileListBox->SelectionMode = SelectionMode::Multiple;
	hBorder2->Child = hChLocalFileListBox;
	hCheckInGrid->SetRow(hBorder2, 0);
	hCheckInGrid->SetColumn(hBorder2, 2);
	hCheckInGrid->Children->Add(hBorder2);

	hChShowItemsButton->Content = "LocalFolder";
	hChShowItemsButton->Height = 30;
	hChShowItemsButton->Width = 100;
	hCheckInGrid->SetRow(hChShowItemsButton, 1);
	hCheckInGrid->SetColumn(hChShowItemsButton, 2);
	hCheckInGrid->Children->Add(hChShowItemsButton);

	hChSelectButton->Content = "Select Files";
	hChSelectButton->Height = 30;
	hChSelectButton->Width = 100;
	hCheckInGrid->SetRow(hChSelectButton, 2);
	hCheckInGrid->SetColumn(hChSelectButton, 2);
	hCheckInGrid->Children->Add(hChSelectButton);
}
/* set row and col of view of check out  */
void CppCliWindows::WPFCppCliDemo::setUpCheckOutViewRowCol()
{
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	RowDefinition^ hRow2Def = gcnew RowDefinition();
	RowDefinition^ hRow3Def = gcnew RowDefinition();
	hCheckOutGrid->RowDefinitions->Add(hRow1Def);
	hCheckOutGrid->RowDefinitions->Add(hRow2Def);
	hCheckOutGrid->RowDefinitions->Add(hRow3Def);
	hRow1Def->Height = GridLength(370);
	hRow2Def->Height = GridLength(50);
	hRow3Def->Height = GridLength(50);

	ColumnDefinition^ hCol1Def = gcnew ColumnDefinition();
	ColumnDefinition^ hCol2Def = gcnew ColumnDefinition();
	ColumnDefinition^ hCol3Def = gcnew ColumnDefinition();
	hCheckOutGrid->ColumnDefinitions->Add(hCol1Def);
	hCheckOutGrid->ColumnDefinitions->Add(hCol2Def);
	hCheckOutGrid->ColumnDefinitions->Add(hCol3Def);
	hCol1Def->Width = GridLength(345);
	hCol2Def->Width = GridLength(50);
	hCol3Def->Width = GridLength(345);
}
void CppCliWindows::WPFCppCliDemo::setUpCheckOutViewPartsButton()
{
	hChOutGetlistButton->Content = "Remote List";
	hChOutGetlistButton->Height = 30;
	hChOutGetlistButton->Width = 100;
	hCheckOutGrid->SetRow(hChOutGetlistButton, 1);
	hCheckOutGrid->SetColumn(hChOutGetlistButton, 0);
	hCheckOutGrid->Children->Add(hChOutGetlistButton);

	hChOutSelectlistButton->Content = "Selecte Files";
	hChOutSelectlistButton->Height = 30;
	hChOutSelectlistButton->Width = 100;
	hCheckOutGrid->SetRow(hChOutSelectlistButton, 2);
	hCheckOutGrid->SetColumn(hChOutSelectlistButton, 0);
	hCheckOutGrid->Children->Add(hChOutSelectlistButton);

	hChClearDownRetButton->Content = "Clear";
	hChClearDownRetButton->Height = 30;
	hChClearDownRetButton->Width = 50;
	hCheckOutGrid->SetRow(hChClearDownRetButton, 2);
	hCheckOutGrid->SetColumn(hChClearDownRetButton, 1);
	hCheckOutGrid->Children->Add(hChClearDownRetButton);
}
/* set up the view for check out process */
void CppCliWindows::WPFCppCliDemo::setUpCheckOutView()
{
	Console::Write("\n  setting up CheckOut view");
	hCheckOutGrid->Margin = Thickness(20);   // add connect button
	hCheckOutTab->Content = hCheckOutGrid;
	setUpCheckOutViewRowCol();

	Border^ hBorder1 = gcnew Border();                  // 
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hChOutRemoteFileListBox->SelectionMode = SelectionMode::Multiple;
	hBorder1->Child = hChOutRemoteFileListBox;
	hCheckOutGrid->SetRow(hBorder1, 0);
	hCheckOutGrid->SetColumn(hBorder1, 0);
	hCheckOutGrid->Children->Add(hBorder1);

	Border^ hBorder2 = gcnew Border();                  // 
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hChOutResultListBox->SelectionMode = SelectionMode::Multiple;
	hBorder2->Child = hChOutResultListBox;
	hCheckOutGrid->SetRow(hBorder2, 0);
	hCheckOutGrid->SetColumn(hBorder2, 2);
	hCheckOutGrid->Children->Add(hBorder2);

	setUpCheckOutViewPartsButton(); // set up parts of buttons

	hChOutWithOutDepsButton->Content = "Down Without Dep";
	hChOutWithOutDepsButton->Height = 30;
	hChOutWithOutDepsButton->Width = 100;
	hCheckOutGrid->SetRow(hChOutWithOutDepsButton, 1);
	hCheckOutGrid->SetColumn(hChOutWithOutDepsButton, 2);
	hCheckOutGrid->Children->Add(hChOutWithOutDepsButton);

	hChOutWithDepsButton->Content = "Down With Dep";
	hChOutWithDepsButton->Height = 30;
	hChOutWithDepsButton->Width = 100;
	hCheckOutGrid->SetRow(hChOutWithDepsButton, 2);
	hCheckOutGrid->SetColumn(hChOutWithDepsButton, 2);
	hCheckOutGrid->Children->Add(hChOutWithDepsButton);	
}
/* load window */
void WPFCppCliDemo::OnLoaded(Object^ sender, RoutedEventArgs^ args)
{
	Console::Write("\n  Window loaded");
}
/* unload window */
void WPFCppCliDemo::Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args)
{
	Console::Write("\n  Window closing");
	pChann_->stop();
	stopRecvThread();
	pRecvr_->stop();
	Console::Write("\n  Channel closing");
	pSendr_->stop();
	Console::Write("\n  Sender closing");
}

[STAThread]
//int _stdcall WinMain()
int main(array<System::String^>^ args)
{
	Console::WriteLine(L"\n Starting WPFCppCliDemo");

	Application^ app = gcnew Application();
	app->Run(gcnew WPFCppCliDemo());
	Console::WriteLine(L"\n\n");
}