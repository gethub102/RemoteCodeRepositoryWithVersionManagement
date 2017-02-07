#ifndef WINDOW_H
#define WINDOW_H
/////////////////////////////////////////////////////////////////////
// Window.h - C++\CLI implementation of WPF Application            //
//          - Demo for CSE 687 Project #4                          //
//  ver 2.1                                                        //
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

using namespace System;
using namespace System::Text;
using namespace System::Windows;
using namespace System::Windows::Input;
using namespace System::Windows::Markup;
using namespace System::Windows::Media;                   // TextBlock formatting
using namespace System::Windows::Controls;                // TabControl
using namespace System::Windows::Controls::Primitives;    // StatusBar
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Windows::Threading;
using namespace System::ComponentModel;
using namespace System::Diagnostics;

#include "../MockChannel/MockChannel.h"
#include <iostream>

namespace CppCliWindows
{
	ref class WPFCppCliDemo : Window
	{
		// MockChannel references

		ISendr* pSendr_;
		IRecvr* pRecvr_;
		IMockChannel* pChann_;

		// Controls for Window

		DockPanel^ hDockPanel = gcnew DockPanel();      // support docking statusbar at bottom //invisable
		Grid^ hGrid = gcnew Grid();                     // one wiget after another, but grid can take many children
		TabControl^ hTabControl = gcnew TabControl();   // contain tab item
		TabItem^ hSendMessageTab = gcnew TabItem();
		TabItem^ hFileListTab = gcnew TabItem();
		TabItem^ hConnectTab = gcnew TabItem();

		TabItem^ hCheckInTab = gcnew TabItem();        // tab for check in process
		TabItem^ hCheckOutTab = gcnew TabItem();       // tab for check Ot process

		StatusBar^ hStatusBar = gcnew StatusBar();
		StatusBarItem^ hStatusBarItem = gcnew StatusBarItem();
		TextBlock^ hStatus = gcnew TextBlock();

		// Controls for SendMessage View
		Grid^ hSendMessageGrid = gcnew Grid();         // grid to contain widgets //invisable
		Button^ hSendButton = gcnew Button();
		Button^ hClearButton = gcnew Button();
		TextBlock^ hTextBlock1 = gcnew TextBlock();
		ScrollViewer^ hScrollViewer1 = gcnew ScrollViewer();
		StackPanel^ hStackPanel1 = gcnew StackPanel();

		String^ msgText
			= "Command:ShowMessage\n"   // command
			+ "Sendr:localhost@8080\n"  // send address
			+ "Recvr:localhost@8090\n"  // receive address
			+ "Content-length:44\n"     // body length attribute
			+ "\n"                      // end header
			+ "Hello World\nCSE687 - Object Oriented Design";  // message body

		// Controls for FileListView View
		Grid^ hFileListGrid = gcnew Grid();
		Forms::FolderBrowserDialog^ hFolderBrowserDialog = gcnew Forms::FolderBrowserDialog();  //
		ListBox^ hListBox = gcnew ListBox();                   // show message 
		Button^ hFolderBrowseButton = gcnew Button();
		Button^ hShowItemsButton = gcnew Button();
		Grid^ hGrid2 = gcnew Grid();

		// Controls for connect
		Grid^ hConnectGrid = gcnew Grid();
		ListBox^ hConnectListBox = gcnew ListBox();
		Button^ hConnectButton = gcnew Button();
		TextBox^ hIpTextBox = gcnew TextBox();
		TextBox^ hPortTextBox = gcnew TextBox();
		TextBox^ hLocalIpTextBox = gcnew TextBox();
		TextBox^ hLocalPortTextBox = gcnew TextBox();
		TextBox^ hStatusTextBox = gcnew TextBox();
		Label^ hIpLable = gcnew Label();
		Label^ hPortLable = gcnew Label();
		Label^ hLocalIpLable = gcnew Label();
		Label^ hLocalPortLable = gcnew Label();
		
		// control for check in 
		Grid^ hCheckInGrid = gcnew Grid();
		ListBox^ hChRemoteFileListBox = gcnew ListBox();
		ListBox^ hChLocalFileListBox = gcnew ListBox();
		Button^ hChGetlistButton = gcnew Button();
		Button^ hChSelectlistButton = gcnew Button();
		Button^ hChCheckInButton = gcnew Button();
		Button^ hChShowItemsButton = gcnew Button();
		Button^ hChSelectButton = gcnew Button();
		Button^ hChClearButton = gcnew Button();

		// control for check out
		Grid^ hCheckOutGrid = gcnew Grid();
		ListBox^ hChOutRemoteFileListBox = gcnew ListBox();
		ListBox^ hChOutResultListBox = gcnew ListBox();
		Button^ hChOutGetlistButton = gcnew Button();
		Button^ hChOutSelectlistButton = gcnew Button();
		Button^ hChOutWithOutDepsButton = gcnew Button();
		Button^ hChOutWithDepsButton = gcnew Button();
		TextBox^ hDownloadStatusTextBox = gcnew TextBox();
		Button^ hChClearDownRetButton = gcnew Button();
		// receive thread

		Thread^ recvThread;
	
	public:
		WPFCppCliDemo();
		~WPFCppCliDemo();
		void setEventHandler();
		void setUpStatusBar();
		void setUpTabControl();
		void setUpSendMessageView();
		void setUpFileListView();
		void setUpConnectionViewRowCol();
		void setUpConnectIpAndPortLable();
		void setUpConnectionView();
		void setUpCheckInViewRowCol();
		void setUpCheckInViewPartsButton();
		void setUpCheckInView();
		void setUpCheckOutViewRowCol();
		void setUpCheckOutViewPartsButton();
		void setUpCheckOutView();

		void sendMessage(Object^ obj, RoutedEventArgs^ args);
		void addText(String^ msg);
		void addGetListText(String^ msg);
		void addDownResultText(String^ msg);
		void getMessage();
		void clear(Object^ sender, RoutedEventArgs^ args);
		void getItemsFromList(Object^ sender, RoutedEventArgs^ args);
		void browseForFolder(Object^ sender, RoutedEventArgs^ args);
		void connectToServer(Object^ sender, RoutedEventArgs^ args);
		void getFileListFromServer(Object^ sender, RoutedEventArgs^ args);
		void selectRemoteFiles(Object^ sender, RoutedEventArgs^ args);
		void browsLocalFiles(Object^ sender, RoutedEventArgs^ args);
		void selectLocalFiles(Object^ sender, RoutedEventArgs^ args);
		void checkIn(Object^ sender, RoutedEventArgs^ args);
		void clearCheckInList(Object^ sender, RoutedEventArgs^ args);
		void getRemoteFilesList(Object^ sender, RoutedEventArgs^ args);
		void selectRemoteFilesForDown(Object^ sender, RoutedEventArgs^ args);
		void checkOutWithOutDep(Object^ sender, RoutedEventArgs^ args);
		void checkOutWithDep(Object^ sender, RoutedEventArgs^ args);
		void clearDownloadResult(Object^ sender, RoutedEventArgs^ args);
		void OnLoaded(Object^ sender, RoutedEventArgs^ args);
		void Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args);

		void stopRecvThread() { stopRecv_ = true; };
	private:
		std::string toStdString(String^ pStr);
		String^ toSystemString(std::string& str);
		void setTextBlockProperties();
		void setButtonsProperties();
		std::string localUrl();
		std::string remoteUrl();
		bool stopRecv_ = false;
	};
}


#endif
