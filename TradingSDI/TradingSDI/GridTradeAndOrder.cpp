#include "stdafx.h"
#include "resource.h"
#include "GridTradeAndOrder.h"
#include <afxdisp.h>
#include <time.h>
#include <locale.h>
#include <MainFrm.h>
#include "document.h"
#include <iostream>
#include "CNetPosTable.h"
#include "Netpos_Get_Data.h"
#include "OverViewOrderGrid.h"
#include "Table_Position_Detail.h"
using namespace std;
using namespace rapidjson;
#import "C:\Program Files\Common Files\System\ADO\msado15.dll" \
no_namespace rename("EOF", "EndOfFile")

#define WM_MY_THREAD_MESSAGE1				WM_APP+200
#define WM_MY_THREAD_MESSAGE_ROWSNO1			WM_APP+201
#define WM_MY_THREAD_MESSAGE_REFRESH1		WM_APP+202
#define GRID_ROWS_COUNT1						WM_APP+203
#define DELETE_ROW1	     					WM_APP+204
#define DELETE_THREAD	     				WM_APP+205
#define ROW_NO_FIND	     					WM_APP+206
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//CUIThread
BEGIN_MESSAGE_MAP(GridTradeAndOrder,CUGCtrl)	
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_MY_THREAD_MESSAGE1, OnThreadMessage)
	ON_MESSAGE(WM_MY_THREAD_MESSAGE_ROWSNO1, OnThreadMessage_RowsNo)		
	ON_MESSAGE(WM_MY_THREAD_MESSAGE_REFRESH1, GridRefresh)		
	ON_MESSAGE(GRID_ROWS_COUNT1, GridRowCount)		
	ON_MESSAGE(DELETE_ROW1, RowDelete)		
	ON_MESSAGE(DELETE_THREAD,DeleteThred)
	ON_MESSAGE(ROW_NO_FIND,rownofind)
END_MESSAGE_MAP()
CString GridTradeAndOrder::m_c_symbol=L"";
int GridTradeAndOrder::value_row_no=0;
int GridTradeAndOrder::filter_break=0;
int GridTradeAndOrder::insertFilterFlag=0;
int GridTradeAndOrder::check_First=0;
int GridTradeAndOrder::thred_killed_ok=0;
_variant_t GridTradeAndOrder::avarRecords;
int GridTradeAndOrder::lock_data=0;
int GridTradeAndOrder::intRows=0;
int GridTradeAndOrder::lock_data_fill=0;
int GridTradeAndOrder::Grid_Rows_Count=0;
int GridTradeAndOrder::data_display=0;
int GridTradeAndOrder::grid_populate_check=0;
_bstr_t GridTradeAndOrder::m_selected_login="0";
_bstr_t GridTradeAndOrder::m_selected_Name="0";
_bstr_t GridTradeAndOrder::strShort("  order by t1.login asc,t1.symbol asc");
_bstr_t GridTradeAndOrder::cellvalue("");
_bstr_t GridTradeAndOrder::strFilter(" ");
_variant_t GridTradeAndOrder::result;
long GridTradeAndOrder::rgIndices[2];
_bstr_t GridTradeAndOrder::bstr_currenttime("");
/////////////////////////////////////////////////////////////////////////////
//Standard MyCug construction/destruction


extern  CLogFile m_logfile_g;
int thread_check=0;

GridTradeAndOrder::GridTradeAndOrder()
{
  
	m_logfile_g.LogEvent(L"Start GridTradeAndOrder");	
	 t = CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");
	 bstr_currenttime=t;
	 GridTradeAndOrder::grid_populate_check=0;
	UGXPThemes::UseThemes(false);
	m_logfile_g.LogEvent(L"End GridTradeAndOrder");
}

GridTradeAndOrder::~GridTradeAndOrder()
{
	m_logfile_g.LogEvent(L"Start GridTradeAndOrder");
	try
	{

    UGXPThemes::CleanUp();
	//WaitForSingleObject(m_pThreads, 100000);
	thread_check=1;
    DWORD exit_code= NULL;
	if (m_pThreads != NULL)
	{
	if(WaitForSingleObject(m_pThreads->m_hThread, INFINITE) != WAIT_OBJECT_0) 
	{
    GetExitCodeThread(m_pThreads->m_hThread, &exit_code);
    if(exit_code == STILL_ACTIVE)
    {
        ::TerminateThread(m_pThreads->m_hThread, 0);
        CloseHandle(m_pThreads->m_hThread);
    }
    m_pThreads->m_hThread = NULL;
    m_pThreads = NULL;
	}
	}
	}
		catch(_com_error & ce)
	{
		AfxMessageBox(ce.Description()+L"Thread UnInitiliaze");			
	}
	
m_logfile_g.LogEvent(L"End GridTradeAndOrder");
   
}

LRESULT GridTradeAndOrder::DeleteThred(WPARAM wParam, LPARAM lParam)
{
	m_logfile_g.LogEvent(L"Start DeleteThred");
	DWORD exit_code= NULL;
	if (m_pThreads != NULL)
	{
    GetExitCodeThread(m_pThreads->m_hThread, &exit_code);
    if(exit_code == STILL_ACTIVE)
    {
        ::TerminateThread(m_pThreads->m_hThread, 0);
        CloseHandle(m_pThreads->m_hThread);
    }
    m_pThreads->m_hThread = NULL;
    m_pThreads = NULL;
	}
	
	GridTradeAndOrder::thred_killed_ok=0;
	m_logfile_g.LogEvent(L"End DeleteThred");
	return 0;
}

UINT update_data_Trade(void*);
UINT update_data_Trade(void *pParam)
{
	m_logfile_g.LogEvent(L"Start update_data_Trade");
	HRESULT hr;
	GridTradeAndOrder* pThis= (GridTradeAndOrder*)pParam;	
	CoInitialize(NULL);
	CDataSource connection;
	CSession session;
	CCommand<CAccessor<CTable_Position_Detail> > artists1;

	connection.OpenFromInitializationString(L"Provider=SQLNCLI11.1;Password=ok@12345;Persist Security Info=False;User ID=sa;Initial Catalog=CHECKDATA;Data Source=68.168.101.187;Use Procedure for Prepare=1;Auto Translate=True;Packet Size=4096;Workstation ID=WINDOWS-LOJSHQK;Initial File Name=\"\";Use Encryption for Data=False;Tag with column collation when possible=False;MARS Connection=False;DataTypeCompatibility=0;Trust Server Certificate=False;Application Intent=READWRITE");
	//connection.OpenFromInitializationString(L"Provider=SQLNCLI11.1;Password=ok@12345;Persist Security Info=False;User ID=sa;Initial Catalog=TRADEDATABASE;Data Source=68.168.101.187;Use Procedure for Prepare=1;Auto Translate=True;Packet Size=4096;Workstation ID=WINDOWS-LOJSHQK;Initial File Name=\"\";Use Encryption for Data=False;Tag with column collation when possible=False;MARS Connection=False;DataTypeCompatibility=0;Trust Server Certificate=False;Application Intent=READWRITE");
	
	session.Open(connection);
	int check_firsttimeval=0;
	while (thread_check==0)
	{	

		if (OverViewOrderGrid::thred_kill==1)
		{
			break;
		}
		Sleep(200);

		
		_bstr_t strMainCode="";
		_bstr_t strComment="";
		CString sel_login=GridTradeAndOrder::m_selected_login;
		CString sel_comment=sel_login.Mid(7,sel_login.GetLength()-7);
		
		sel_login=sel_login.Mid(0,6);

		if (sel_comment==L"--")
		{
			sel_comment=L"";
			GridTradeAndOrder::m_selected_login=sel_login;
		}
		strMainCode=sel_login;
		strComment=sel_comment;

		_bstr_t strCommand="";		
		
		strCommand="select vt1.symbol,'' as 'order',vt3.lastTime,case when vt1.netqty>0 then 'BUY' when vt1.netqty<0 then 'SELL' else '' end  as 'TYPE',vt1.netqty as 'VOLUME',vt1.avgrate as 'Price',vt2.last_tick as 'CURRENT_RATE' ,case when vt1.netqty<0 then (isnull(vt2.last_tick,0)-vt1.avgrate)*vt1.netqty*vt1.contractSize when vt1.netqty>0 then (isnull(vt2.last_tick,0)-vt1.avgrate)*vt1.netqty*vt1.contractSize else 0  end as 'PL',vt3.[status],vt6.TotalTrade,vt7.Checked_time,vt8.limit,vt7.REMARK2  from  (select PositionAverageAccounting.Symbol,PositionAverageAccounting.multiplayer as 'contractSize',totalvolume/10000 as 'NetQty', WavgPrice as 'AVGRate' from PositionAverageAccounting where  PositionAverageAccounting.[login]='" + GridTradeAndOrder::m_selected_login + "' and totalvolume<>0   )vt1 left outer join (select t1.symbol,last_tick.last_tick from last_tick left outer join symbol_mapping as t1 on t1.mapping_Symbol=last_tick.symbol) vt2 on vt1.symbol=vt2.symbol left outer join (select t1.symbol,t1.LastTime,t1.Last_Deal,case when ([type]=0 or [type]=1) then 'MARKET' else 'LIMIT' end  as 'STATUS' from (select symbol,max([time]) 'LastTime',max([order])  as 'Last_Deal' from  Deal_Table_Accounting where Deal_Table_Accounting.[login]='" + GridTradeAndOrder::m_selected_login + "'   group by Deal_Table_Accounting.Symbol ) t1 left outer join mt5_orders_history on t1.last_deal=mt5_orders_history.[order])vt3 on vt1.symbol=vt3.symbol left outer join (select [symbol],cast(sum(case when col1=1 then col2 else 0 end) as varchar(20))+'/' +cast(sum(col2) as varchar(20)) as 'TotalTrade' from  (select [symbol],isnull(checked,0) as 'Col1',count (isnull(checked,0)) as 'Col2' from mt5_deals left outer join Trade_Checked on mt5_deals.Deal=Trade_Checked.Deal  LEFT  outer join client on client.V_login=cast(mt5_deals.[login]  as varchar(20))  where (cast(mt5_deals.[login] as varchar(50))='" + GridTradeAndOrder::m_selected_login + "' and comment='' and  isnull(Comment_YN,'N') ='Y')  or (cast(mt5_deals.[login] as varchar(50))='" + GridTradeAndOrder::m_selected_login + "' and isnull(Comment_YN,'N') <>'Y') group by [symbol],isnull(checked,0) )t1 group by [symbol])vt6 on vt6.symbol=vt1.symbol  left outer join (select * from Position_Check where [login]='" + GridTradeAndOrder::m_selected_login + "')vt7 on vt1.symbol=vt7.symbol left outer join ( select [login],symbol,cast(isnull(limit,0) as varchar(20))+'/'+cast(isnull(LimitSell,0) as varchar(20)) as limit from Limit_Mapping where [login]='" + GridTradeAndOrder::m_selected_login + "')vt8 on  vt1.symbol=vt8.symbol";
		char* strCommand_char=(char*)strCommand;
		hr=artists1.Open(session,strCommand_char);				
		if (OverViewOrderGrid::thred_kill==0)
		{	 
		pThis->SendMessage(GRID_ROWS_COUNT1, 1,1);			 		 		
		}		 
		 int intRows=0;
		 
		 double t_d_m_Pre_NetQty=0;
		 double t_d_m_Diff_NetQty=0;
		 double t_d_m_NetQty=0;
		 double t_d_m_PL=0;
		 double t_d_m_Balance=0;
		 if(SUCCEEDED(hr))
		{
		 while (artists1.MoveNext() == S_OK)
		 {						
				if (GridTradeAndOrder::Grid_Rows_Count<=intRows)
				{
					if (OverViewOrderGrid::thred_kill==0)
					{
					pThis->SendMessage(WM_MY_THREAD_MESSAGE_ROWSNO1, intRows,1);			 		 
					}
				
				}						
				for (int cc=0;cc<13;cc++)
				{										    
					 if (cc==0)
					{
						GridTradeAndOrder::cellvalue=artists1.m_Symbol;				 
					}
					else if(cc==1)
					{
						GridTradeAndOrder::cellvalue=artists1.m_Order;				 
					}
					else if(cc==2)
					{
						 GridTradeAndOrder::cellvalue=artists1.m_Time;				 
					}
					else if(cc==3)
					{
						GridTradeAndOrder::cellvalue=artists1.m_Type;				 						
						
					}
					else if(cc==4)
					{
						GridTradeAndOrder::cellvalue=artists1.m_Volume;				 
						
					}
					else if(cc==5)
					{
						 
						LPTSTR endPtr;
						double d_m_PL = _tcstod(artists1.m_Price, &endPtr);
												
						CString cstrpl;
						cstrpl.Format(_T("%.2f"),d_m_PL);												
						GridTradeAndOrder::cellvalue=cstrpl;
					}
					else if(cc==6)
					{

						LPTSTR endPtr;
						double d_m_PL = _tcstod(artists1.m_Current_Rate, &endPtr);
												
						CString cstrpl;
						cstrpl.Format(_T("%.2f"),d_m_PL);												
						GridTradeAndOrder::cellvalue=cstrpl;												
					}
					else if(cc==7)
					{
						LPTSTR endPtr;
						double d_m_PL = _tcstod(artists1.m_PL, &endPtr);
												
						CString cstrpl;
						cstrpl.Format(_T("%.0f"),d_m_PL);												
						GridTradeAndOrder::cellvalue=cstrpl;						
					}
					else if(cc==8)
					{
						GridTradeAndOrder::cellvalue=artists1.m_Status;				 
						
					}
					else if(cc==9)
					{
						GridTradeAndOrder::cellvalue=artists1.m_Trade_Checked;				 
						
					}
					else if(cc==10)
					{
						GridTradeAndOrder::cellvalue=artists1.m_Checked_Time ;				 
						
					}
					else if(cc==11)
					{
						GridTradeAndOrder::cellvalue=artists1.m_Limit ;	
						if(check_firsttimeval==0)
						{
							if (OverViewOrderGrid::thred_kill==0)
							{
								
								 pThis->SendMessage(WM_MY_THREAD_MESSAGE1, cc,intRows);
							}
						}
						
					}
				    else if(cc==12)
					{
						GridTradeAndOrder::cellvalue=artists1.m_Remark2  ;	
						if(check_firsttimeval==0)
						{
							if (OverViewOrderGrid::thred_kill==0)
							{
							pThis->SendMessage(WM_MY_THREAD_MESSAGE1, cc,intRows);
							}
						}
						
					}

					if (cc!=12 && cc!=11)
					{
						if (OverViewOrderGrid::thred_kill==0)
						{
							
							pThis->SendMessage(WM_MY_THREAD_MESSAGE1, cc,intRows);	
						}
					}
										
					
					GridTradeAndOrder::cellvalue="";
					
				}
				intRows=intRows+1;
			
		  }
		 }
		 int actual_row=intRows;

		
		 
			if (GridTradeAndOrder::Grid_Rows_Count<intRows)
			{
				 if (GridTradeAndOrder::insertFilterFlag!=0)
				 {
					 if (OverViewOrderGrid::thred_kill==0)
					{
						pThis->SendMessage(WM_MY_THREAD_MESSAGE_ROWSNO1, intRows+1,1);	
					 }
				 }
				 else
				 {
					 if (OverViewOrderGrid::thred_kill==0)
					{
					 pThis->SendMessage(WM_MY_THREAD_MESSAGE_ROWSNO1, intRows,1);	
					 }
				 }												
			}							
		 _bstr_t str="";
		 CString  cstr=L"";
		 for (int cc=0;cc<12;cc++)
		 {			
			 if (OverViewOrderGrid::thred_kill==0)
			{
				pThis->SendMessage(WM_MY_THREAD_MESSAGE1, cc,intRows);	
			 }
		 }
		 intRows=intRows+1;

		  if (GridTradeAndOrder::insertFilterFlag!=0)
			{				
				intRows=intRows+1;
			}
		 for (intRows;intRows<GridTradeAndOrder::Grid_Rows_Count;intRows++)
		 {
			 if (OverViewOrderGrid::thred_kill==0)
			{
				pThis->SendMessage(DELETE_ROW1, intRows,1);	
			 }
		 }		 
		 artists1.Close();	
		 GridTradeAndOrder::grid_populate_check=1;
		 if (OverViewOrderGrid::thred_kill==0)
		{
		 pThis->SendMessage(WM_MY_THREAD_MESSAGE_REFRESH1, 1,1);	
		 }
		 GridTradeAndOrder::data_display=0;		
		 check_firsttimeval=1;
	}
	session.Close();
	connection.Close();
	m_logfile_g.LogEvent(L"End update_data_Trade");
    return 0;
	
}



LRESULT GridTradeAndOrder::GridRefresh(WPARAM wParam, LPARAM lParam)
{
	RedrawAll();
	return 0;
}

LRESULT GridTradeAndOrder::rownofind(WPARAM wParam, LPARAM lParam)
{
m_logfile_g.LogEvent(L"Start rownofind");
CString m_symbol=L"";
if(OverViewOrderGrid::thred_kill==0)
  {
	for(int f=0;f<GetNumberRows();f++)
	{
		
		m_symbol=QuickGetText(0,f);
		if ( wcscmp(m_c_symbol,m_symbol)==0)
		{			
			value_row_no=f;
			return value_row_no;
		}
	}
	value_row_no=0;

	if (value_row_no==0)
	{
		for(int f=0;f<GetNumberRows();f++)
		{
			m_symbol=QuickGetText(0,f);
			if (wcscmp(m_symbol,L"")==0)
			{			
				value_row_no=f;
				return value_row_no;
			}
		}
	}

 }
m_logfile_g.LogEvent(L"End rownofind");
  return value_row_no;
}



LRESULT GridTradeAndOrder::OnThreadMessage(WPARAM wParam, LPARAM lParam)
{
	m_logfile_g.LogEvent(L"Start GridTradeOrder Onthreaad msg");
	HRESULT hr = S_OK;	
	int col= (int)wParam;
	int row= (int)lParam;
	
	if (GridTradeAndOrder::insertFilterFlag==0)
	{
		
		QuickSetText(col,row,GridTradeAndOrder::cellvalue);	
		/*if (GridTradeAndOrder::grid_populate_check==0)
		{*/
			CUGCell cell ;	
			/*GetColDefault( 12, &cell );			
			if (cell.GetCellType()!=UGCT_CHECKBOX )
			{
				QuickSetCellType(12,row,UGCT_CHECKBOX);	
				QuickSetCellTypeEx(12,row,UGCT_CHECKBOXCHECKMARK);
				SetColDefault( 12, &cell );
			}*/

			
			GetCell(11,0,&cell);
			if (cell.GetParam()!=CELLTYPE_IS_EDITABLE)
			{
				cell.SetParam(CELLTYPE_IS_EDITABLE);
			}
			GetCell(12,0,&cell);
			if (cell.GetParam()!=CELLTYPE_IS_EDITABLE)
			{
				cell.SetParam(CELLTYPE_IS_EDITABLE);
			}
		//}
	}
	else
	{
		QuickSetText(col,row+1,GridTradeAndOrder::cellvalue);	
			
		//RedrawCell(col,row+1);
	}
	m_logfile_g.LogEvent(L"End GridTradeOrder Onthreaad msg");
	return 0;
	
}

LRESULT GridTradeAndOrder::GridRowCount(WPARAM wParam, LPARAM lParam)
{
	//RedrawAll();
	GridTradeAndOrder::Grid_Rows_Count=GetNumberRows();
	return 0;
}

LRESULT GridTradeAndOrder::RowDelete(WPARAM wParam, LPARAM lParam)
{
	m_logfile_g.LogEvent(L"Start RowDelete");
	int row= (int)wParam;
	for(int f=0;f<13;f++)
	{
		QuickSetText(f,row,L"");
	}
	m_logfile_g.LogEvent(L"End RowDelete");
	//DeleteRow(row);
	return 0;
}



LRESULT GridTradeAndOrder::OnThreadMessage_RowsNo(WPARAM wParam, LPARAM lParam)
{
	m_logfile_g.LogEvent(L"Start OnThreadMessage_RowsNo");
	int val= (int)wParam;
	int flag= (int)lParam;
	if(flag==0)
	{
		SetNumberRows(val);
	}
	if(flag==1)
	{
		InsertRow(val);
	}
		m_logfile_g.LogEvent(L"End OnThreadMessage_RowsNo");
		return 0;
}







/////////////////////////////////////////////////////////////////////////////
//	OnSheetSetup	
//		This notification is called for each additional sheet that the grid
//		might contain, here you can customize each sheet in the grid.
//	Params:
//		sheetNumber - idndex of current sheet
//	Return:
//		<none>
void GridTradeAndOrder::OnSheetSetup(int sheetNumber)
{
	m_logfile_g.LogEvent(L"Start OnSheetSetup");
	int	nRow = 0, nCol = 0;
	// ****************************************************************
	// ** Set up columns
	// ****************************************************************
	CUGCell cell;

	GetGridDefault(&cell);
	//cell.SetBackColor(MY_COLOR_BACK);
	//cell.SetTextColor(MY_COLOR_TEXT);
	SetGridDefault(&cell);
	EnableExcelBorders(TRUE);
	SetHighlightRow(TRUE, FALSE);
	SetDoubleBufferMode(TRUE);
	SetDefFont(0);
	SetSH_Width(0);
	
	// Number
			SetNumberCols(13);
			QuickSetText(0,-1,L"Symbol");
			SetColWidth(0,100);
			QuickSetText(1,-1,L"Order");
			SetColWidth(1,50);
			QuickSetText(2,-1,L"Time");	
			SetColWidth(2,140);
			
			QuickSetText(3,-1,L"Type");
			SetColWidth(3,60);
			
			QuickSetText(4,-1,L"Volume");
			SetColWidth(4,40);
			QuickSetText(5,-1,L"Price");	
			SetColWidth(5,60);
			QuickSetText(6,-1,L"LTP Price");
			SetColWidth(6,60);
			QuickSetText(7,-1,L"P&L");
			SetColWidth(7,70);
			
			QuickSetText(8,-1,L"Status");
			SetColWidth(8,60);		

			QuickSetText(9,-1,L"Checked Status");
			SetColWidth(9,80);		

			QuickSetText(10,-1,L"Checked Time");
			SetColWidth(10,130);

			QuickSetText(11,-1,L"Alloted Limit");
			SetColWidth(11,100);		

			QuickSetText(12,-1,L"Remark2");
			SetColWidth(12,100);		
			
	// Header font
	for(int i = 0; i < GetNumberCols(); i++)
	{
		QuickSetFont(i, -1, 1);
	}
m_logfile_g.LogEvent(L"End OnSheetSetup");
}

		

void GridTradeAndOrder::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	m_logfile_g.LogEvent(L"Start OnDClicked");
	

	//CDialog mySettings( IDD_TAB_DATA );
   //INT_PTR returnCode = -1;
   //returnCode = mySettings.DoModal();

   
	CUGCell cell;
	GetCell(col,row,&cell);
	int nCellTypeIndex = cell.GetCellType();
	int nParam = cell.GetParam();
	CString string;
	if(cell.GetLabelText() != NULL)
		string = cell.GetLabelText();

	if(processed){
		if(cell.GetCellType() == m_nSpinIndex)
			return ;
	}

	if(nParam == CELLTYPE_IS_EDITABLE || string == "CELLTYPE_IS_EDITABLE"||col==11||col==12)
	{
		StartEdit();
	}
	m_logfile_g.LogEvent(L"End OnDClicked");
}	
			 

void GridTradeAndOrder::OnTH_LClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed)
{
	m_logfile_g.LogEvent(L"Start OnTH_LClicked");
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(rect);
	UNREFERENCED_PARAMETER(point);
	UNREFERENCED_PARAMETER(processed);

	if( updn == 0)
		return;

	QuickSetCellType( m_iSortCol, -1, 0 );

	if( col == m_iSortCol )
	{
		if( m_bSortedAscending )
			m_bSortedAscending = FALSE;
		else
			m_bSortedAscending = TRUE;
	}
	else
	{
		m_iSortCol = col;
		m_bSortedAscending = TRUE;
	}

	if( m_bSortedAscending )
	{
		SortBy( col, UG_SORT_ASCENDING );
		QuickSetCellType( m_iSortCol, -1, m_iArrowIndex );
		QuickSetCellTypeEx( m_iSortCol, -1, UGCT_SORTARROWDOWN );

//		((CFindAndSortDemo*)GetParent())->Trace( _T( "Sorted column %d ascending" ), iCol );
	}
	else
	{
		SortBy( col, UG_SORT_DESCENDING );
		QuickSetCellType( m_iSortCol, -1, m_iArrowIndex );
		QuickSetCellTypeEx( m_iSortCol, -1, UGCT_SORTARROWUP );
//		Trace( _T( "Sorted column %d descending" ), iCol );
	}
	
	RedrawAll();
	
m_logfile_g.LogEvent(L"End OnTH_LClicked");
}
int GridTradeAndOrder::OnCellTypeNotify(long ID,int col,long row,long msg,long param)
{

	if(ID == UGCT_DROPLIST){
		return OnDropList(ID, col, row, msg, param);
	}
}
int GridTradeAndOrder::OnDropList(long ID,int col,long row,long msg,long param)
{
	
	m_logfile_g.LogEvent(L"Start OnDropList");
	if (msg==103)
	{
		if(GridTradeAndOrder::insertFilterFlag==1 && row==0)
	{
		GridTradeAndOrder::filter_break=1;
		check_First==0;
		CString  strval=L"";
		CUGCell cell;
		GetCell(col,row,&cell);
		strval=cell.GetText();
		GridTradeAndOrder::strFilter="";
		if (strval!=L"")
		{
			gridFilter(col,GetNumberRows(),strval);
		}
	}
	RedrawAll();
	}
	m_logfile_g.LogEvent(L"End OnDropList");
	return true;
	
}


/////////////////////////////////////////////////////////////////////////////
//	OnEditFinish
//		This notification is sent when the edit is being finised
//	Params:
//		col, row	- coordinates of the edit cell
//		edit		- pointer to the edit control
//		string		- actual string that user typed in
//		cancelFlag	- indicates if the edit is being cancelled
//	Return:
//		TRUE - to allow the edit it proceede
//		FALSE - to force the user back to editing of that same cell

_bstr_t GridTradeAndOrder::get_string(CString  MainStr,CString SepStr)
{
	m_logfile_g.LogEvent(L"Start get_string");
	int str_len=MainStr.GetLength();
	CString strcode=L"";	
	_bstr_t b_strreturn="";
	_bstr_t b_strCode="";
	_bstr_t b_strChar="";
	_bstr_t bstrcomp="";
	for(int forcount=0;forcount<str_len;forcount++)
	{
		CString strchar=MainStr.Mid(forcount,1);
		if (strchar!=SepStr)
		{
			b_strChar=strchar;
			b_strCode=b_strCode+b_strChar;
		}
		else
		{
			if(b_strreturn!=bstrcomp)
			{
				b_strreturn=b_strreturn+","+"'"+b_strCode+"'";
			}
			else
			{
				b_strreturn="'"+b_strCode+"'";
			}
			b_strCode="";
		}
	}
	
	if (b_strreturn!=bstrcomp)
	{
		b_strreturn=b_strreturn+","+"'"+b_strCode+"'";
	}
	else
	{
		b_strreturn="'" +b_strCode +"'";
	}

	b_strreturn="("+b_strreturn+")";


	m_logfile_g.LogEvent(L"End  get_string");
	return b_strreturn;

}



int GridTradeAndOrder::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag)
{
	m_logfile_g.LogEvent(L"Start  OnEditFinish");
	if(GridTradeAndOrder::insertFilterFlag==1 && row==0)
	{
		GridTradeAndOrder::filter_break=1;
		check_First==0;
		CString  strval=L"";
		CUGCell cell;
		GetCell(col,row,&cell);
		strval=cell.GetText();
		GridTradeAndOrder::strFilter="";
		if (strval!=L"")
		{
			gridFilter(col,GetNumberRows(),strval);
		}
	}
	RedrawAll();

	m_logfile_g.LogEvent(L"End  OnEditFinish");
	return true;
	
}


/////////////////////////////////////////////////////////////////////////////
//	OnMenuCommand
//		This notification is called when the user has selected a menu item
//		in the pop-up menu.
//	Params:
//		col, row - the cell coordinates of where the menu originated from
//		setcion - identify for which portion of the gird the menu is for.
//				  possible sections:
//						UG_TOPHEADING, UG_SIDEHEADING,UG_GRID
//						UG_HSCROLL  UG_VSCROLL  UG_CORNERBUTTON
//		item - ID of the menu item selected
//	Return:
//		<none>
void GridTradeAndOrder::OnMenuCommand(int col,long row,int section,int item)
{
	m_logfile_g.LogEvent(L"Start  OnMenuCommand");
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(section);
	// init. local variables
	CMenu* pMenu;
	pMenu = GetPopupMenu();
	
	switch( item )
	{
		
		case 2000:
		{
			FindDialog();
//			Trace( _T( "Displayed the grid's Find Dialog." ) );
			break;
		}
		case 2001:
		{
			filter();
//			Trace( _T( "Displayed the grid's Replace Dialog." ) );
			break;
		}
		
	}

	m_logfile_g.LogEvent(L"End  OnMenuCommand");
}




/////////////////////////////////////////////////////////////////////////////
//	OnSortEvaluate
//		Sent as a result of the 'SortBy' call, this is called for each cell
//		comparison and allows for customization of the sorting routines.
//		We provide following code as example of what could be done here,
//		you migh have to modify it to give your application customized sorting.
//	Params:
//		cell1, cell2	- pointers to cells that are compared
//		flags			- identifies sort direction
//	Return:
//		value less than zero to identify that the cell1 comes before cell2
//		value equal to zero to identify that the cell1 and cell2 are equal
//		value greater than zero to identify that the cell1 comes after cell2
int GridTradeAndOrder::OnSortEvaluate(CUGCell *cell1,CUGCell *cell2,int flags)
{
	m_logfile_g.LogEvent(L"Start  OnSortEvaluate");
	if( flags & UG_SORT_DESCENDING )
	{
		CUGCell *ptr = cell1;
		cell1 = cell2;
		cell2 = ptr;
	}

	
	COleDateTime date1, date2;
    int retVal = 0;
    switch ( m_iSortCol )
	{
	case 2:
	date1.ParseDateTime( cell1->GetText(), VAR_DATEVALUEONLY );
	date2.ParseDateTime( cell2->GetText(), VAR_DATEVALUEONLY );

	// If a date is invalid, move it to the bottom, by always making it >
	// ( the valid code is 0 )
	if (date1.GetStatus())
	return 1;
	if (date2.GetStatus())
	return -1;
		
	if( date1 < date2 )
	retVal = -1;
	if( date1 > date2 )
	retVal = 1;

	break;
	case 4:
	if( cell1->GetNumber() < cell2->GetNumber())
	retVal = -1;
	if( cell1->GetNumber() > cell2->GetNumber())
	retVal = 1;
			

	break;

	case 5:
	if( cell1->GetNumber() < cell2->GetNumber())
	retVal = -1;
	if( cell1->GetNumber() > cell2->GetNumber())
	retVal = 1;
			

	break;

	case 6:
	if( cell1->GetNumber() < cell2->GetNumber())
	retVal = -1;
	if( cell1->GetNumber() > cell2->GetNumber())
	retVal = 1;
			

	break;

	case 7:
	if( cell1->GetNumber() < cell2->GetNumber())
	retVal = -1;
	if( cell1->GetNumber() > cell2->GetNumber())
	retVal = 1;
			

	break;
   
	case 9:
	if( cell1->GetNumber() < cell2->GetNumber())
	retVal = -1;
	if( cell1->GetNumber() > cell2->GetNumber())
	retVal = 1;
			

	break;
	case 10:
	date1.ParseDateTime( cell1->GetText(), VAR_DATEVALUEONLY );
	date2.ParseDateTime( cell2->GetText(), VAR_DATEVALUEONLY );

	// If a date is invalid, move it to the bottom, by always making it >
	// ( the valid code is 0 )
	if (date1.GetStatus())
	return 1;
	if (date2.GetStatus())
	return -1;
		
	if( date1 < date2 )
	retVal = -1;
	if( date1 > date2 )
	retVal = 1;

	break;

	default:
		if( CString(cell1->GetText()) == "" )
			return 1;
		else if( CString(cell2->GetText()) == "" )
			return -1;

		switch( cell1->GetDataType() )
		{
			case UGCELLDATA_NUMBER:
			case UGCELLDATA_BOOL:
			case UGCELLDATA_CURRENCY:
				if( cell1->GetNumber() < cell2->GetNumber())
					retVal = -1;
				if( cell1->GetNumber() > cell2->GetNumber())
					retVal = 1;

				break;

			default:
				retVal = _tcscmp( cell1->GetText( ), cell2->GetText());
		}
	}
	m_logfile_g.LogEvent(L"End  OnSortEvaluate");
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////
//	OnTabSelected
//		Called when the user selects one of the tabs on the bottom of the screen
//	Params:
//		ID	- id of selected tab
//	Return:
//		<none>
void GridTradeAndOrder::OnTabSelected(int ID)
{
	m_logfile_g.LogEvent(L"Start  OnTabSelected");
	SetSheetNumber(ID);
	PostMessage(WM_SIZE, 0, 0L);
	m_logfile_g.LogEvent(L"End  OnTabSelected");
}

/////////////////////////////////////////////////////////////////////////////
//	OnAdjustComponentSizes
//		Called when the grid components are baing arranged.
//	Params:
//		grid, topHdg, sideHdg, cnrBtn, vScroll, hScroll, 
//		tabs	- sizes and location of each of the grid components
//	Return:
//		<none>

void GridTradeAndOrder::OnSize(UINT nType, int cx, int cy)
{
	
	/*EnableUpdate(FALSE);

	RECT rect;
	GetClientRect(&rect);

	SetTabWidth(max(rect.right/3, 75));
	SetSH_Width(0);
	SetColWidth(0, 175);
	for(int y = 1;y < 14; y++)
	{
		SetColWidth(y, (rect.right-175)/6);
	}

	EnableUpdate(TRUE);


	CUGCtrl::OnSize(nType,cx,cy);*/
}







void GridTradeAndOrder::OnTimer(UINT nIDEvent)
{		
}





 void GridTradeAndOrder::InitMenu()
{
	CMenu submenu;
	CMenu * menu = GetPopupMenu();
	
	EnableMenu(TRUE);
	
	
	AddMenuItem( 2001, _T( "Filter" ) );
	AddMenuItem( 2000, _T( "Find Dialog" ) );
	
	
}

 int GridTradeAndOrder::OnMenuStart( int iCol, long lRow, int iSection )
{
	UNREFERENCED_PARAMETER(iCol);
	UNREFERENCED_PARAMETER(lRow);

	if( iSection == UG_GRID )
	{
//		Trace( _T( "Displayed the grid's internal pop-up menu." ) );	
		return TRUE;
	}
	else
		return FALSE;
}

 void GridTradeAndOrder::filter()
 {
	 m_logfile_g.LogEvent(L"Start  filter");
	if (GridTradeAndOrder::insertFilterFlag==0)
	{
			addItemToCombobox();
			GridTradeAndOrder::insertFilterFlag=1;
			InsertRow(0);
			for (int col_count=0;col_count<13;col_count++)
			{
				CUGCell cell;
				int row=0;
				int col=col_count;
				cell.ClearAll();
				cell.SetCellType(UGCT_DROPLIST);
				cell.SetCellTypeEx(UGCT_DROPLISTHIDEBUTTON);
				cell.SetParam(CELLTYPE_IS_EDITABLE);
				QuickSetRange(col,row,col,row,&cell);
				QuickSetLabelText(col,row,str[col_count]);
			}
	}
	else
	{
		
		DeleteRow(0);
		GridTradeAndOrder::insertFilterFlag=0;
		GridTradeAndOrder::strFilter=" ";
	}
	RedrawAll();

	m_logfile_g.LogEvent(L"End  filter");
 }



BOOLEAN  GridTradeAndOrder::CheckvalueInArray(const CStringArray& arr,CString strval) 
{
	m_logfile_g.LogEvent(L"Start  CheckvalueInArray");
	try
	{
	int arrcount=arr.GetCount();
    for (int forcount=0;forcount<arrcount;forcount++)
	{
		if (arr[forcount]==strval)
		{
			return true;
		}
	}
	m_logfile_g.LogEvent(L"End  CheckvalueInArray");
    return false  ;
	}
	catch(_com_error & ce)
			{
				AfxMessageBox(ce.Description()+L"CheckvalueInArray");			
			} 
}
 
void GridTradeAndOrder::addItemToCombobox()
{
	m_logfile_g.LogEvent(L"Start  CheckvalueInArray");
	CStringArray arr;
	CStringArray arr1;
	CStringArray arr2;
	CStringArray arr3;
	CStringArray arr4;
	CStringArray arr5;
	CStringArray arr6;
	CStringArray arr7;
	CStringArray arr8;
	CStringArray arr9;
	CStringArray arr10;
	CStringArray arr11;
	CStringArray arr12;
	CStringArray arr13;
	try
	{
	int rows=1;
	rows=GetNumberRows();
	
	CString str_val=L"";
	
	for (int forcount=0;forcount<13;forcount++)
	{
		str[forcount]=L"ALL\n";		
	}
	for (int forcount=0;forcount<rows;forcount++)
	{
		for (int clocount=0;clocount<13;clocount++)
		{
			str_val=QuickGetText(clocount,forcount);
			str_val=str_val.Trim();
			
			if (str_val!=L"")
			{
				if (clocount==0)
				{
					if (CheckvalueInArray(arr,str_val)==false )
					{
						str[clocount]=str[clocount]+str_val+L"\n";										
						arr.Add(str_val);
					}
				}


				if (clocount==1)
				{
					if (CheckvalueInArray(arr1,str_val)==false )
					{
						str[clocount]=str[clocount]+str_val+L"\n";										
						arr1.Add(str_val);
					}
				}


				if (clocount==2)
				{
					if (CheckvalueInArray(arr2,str_val)==false )
					{
						str[clocount]=str[clocount]+str_val+L"\n";										
						arr2.Add(str_val);
					}
				}



				if (clocount==3)
				{
					if (CheckvalueInArray(arr3,str_val)==false )
					{
						str[clocount]=str[clocount]+str_val+L"\n";										
						arr3.Add(str_val);
					}
				}

				if (clocount==4)
				{
					if (CheckvalueInArray(arr4,str_val)==false )
					{
						str[clocount]=str[clocount]+str_val+L"\n";										
						arr4.Add(str_val);
					}
				}
				if (clocount==5)
				{
					if (CheckvalueInArray(arr5,str_val)==false )
					{
						str[clocount]=str[clocount]+str_val+L"\n";										
						arr5.Add(str_val);
					}
				}
				if (clocount==6)
				{
					if (CheckvalueInArray(arr6,str_val)==false )
					{
						str[clocount]=str[clocount]+str_val+L"\n";										
						arr6.Add(str_val);
					}
				}
				if (clocount==7)
				{
					if (CheckvalueInArray(arr7,str_val)==false )
					{
						str[clocount]=str[clocount]+str_val+L"\n";										
						arr7.Add(str_val);
					}
				}
				if (clocount==8)
				{
					if (CheckvalueInArray(arr8,str_val)==false )
					{
						str[clocount]=str[clocount]+str_val+L"\n";										
						arr8.Add(str_val);
					}
				}
				if (clocount==9)
				{
					if (CheckvalueInArray(arr9,str_val)==false )
					{
						str[clocount]=str[clocount]+str_val+L"\n";										
						arr9.Add(str_val);
					}
				}
				if (clocount==10)
				{
					if (CheckvalueInArray(arr10,str_val)==false )
					{
						str[clocount]=str[clocount]+str_val+L"\n";										
						arr10.Add(str_val);
					}
				}
				if (clocount==11)
				{
					if (CheckvalueInArray(arr11,str_val)==false )
					{
						str[clocount]=str[clocount]+str_val+L"\n";										
						arr11.Add(str_val);
					}
				}
				if (clocount==12)
				{
					if (CheckvalueInArray(arr12,str_val)==false )
					{
						str[clocount]=str[clocount]+str_val+L"\n";										
						arr12.Add(str_val);
					}
				}
				if (clocount==13)
				{
					if (CheckvalueInArray(arr13,str_val)==false )
					{
						str[clocount]=str[clocount]+str_val+L"\n";										
						arr13.Add(str_val);
					}
				}
				

			}

		}												
	}
	}
	catch(_com_error & ce)
			{
				AfxMessageBox(ce.Description()+L"addItemToCombobox");			

	} 

	m_logfile_g.LogEvent(L"End  CheckvalueInArray");
}
 








void GridTradeAndOrder::OnSetup()
{
	m_logfile_g.LogEvent(L"Start  OnSetup()");
	// Set up the Tab controls
	c1_click=0;
	c2_click=0;
	c3_click=0;
	c4_click=0;
	c5_click=0;
	c6_click=0;
	c7_click=0;
	c8_click=0;
	c9_click=0;
	c10_click=0;
	c11_click=0;
	c12_click=0;
	c13_click=0;

	int	nIndex = 0;
	CUGCell cell;
	GetGridDefault(&cell);
	SetGridDefault(&cell);
	EnableExcelBorders(FALSE);
	SetHighlightRow(TRUE, FALSE);
	SetDoubleBufferMode(TRUE);
	SetUserSizingMode( TRUE );

	
	InitMenu();
	// Create and set default font
	AddFont(L"Arial", -12, FW_NORMAL);
	AddFont(L"Arial", -12, FW_BOLD);
	SetDefFont(0);

	


	CDC * pDC;
	
	int iHeight;
	int i, j;
	
	pDC = GetDC( );
	
	iHeight = -MulDiv( 8, GetDeviceCaps( pDC->m_hDC, LOGPIXELSY), 72 );
	m_font.CreateFont( iHeight, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, _T( "MS Sans Serif" ) );
	
	ReleaseDC( pDC );

	SetDefFont( &m_font );
		
	GetGridDefault( &cell );
	cell.SetAlignment( UG_ALIGNCENTER | UG_ALIGNVCENTER );
	SetGridDefault( &cell );

	SetNumberCols( 14 );
	//SetNumberRows( 50 );

	m_iArrowIndex = AddCellType( &m_sortArrow );

	SetCurrentCellMode( 2 );

	UseHints( TRUE );

	SetSH_Width( 0 );
	
	 // Start timers
	//srand( (unsigned)time( NULL ) );
	SetTimer(0, 100, NULL);
	SetTimer(1, 100, NULL);

	

	QuickSetCellType( 0, -1, m_iArrowIndex );
	SortBy( 0, UG_SORT_ASCENDING );

	m_iSortCol = 0;
	m_bSortedAscending = TRUE;



	m_pThreads=AfxBeginThread(update_data_Trade, this);	
	m_pThreads->m_bAutoDelete=false;
	//m_SocketThred=AfxBeginThread(update_data_Order, this);		


		
		run_check=0;		

		m_logfile_g.LogEvent(L"End  OnSetup()");
}

void GridTradeAndOrder::gridFilter(int colno,int rows_count,CString col_value)
{
	m_logfile_g.LogEvent(L"Start gridFilter");
	CString getColvalue=L"";
	
	for(int fcount=rows_count-1;fcount>0;fcount--)
	{
		getColvalue=QuickGetText(colno,fcount);


		col_value=col_value.Trim();
		CString fcar=col_value.Mid(0,1);
		CString comp_value=col_value.Mid(1,col_value.GetLength()-1);
		if(fcar==L">"||fcar==L"<")
		{
			if (fcar==L">")
			{
				if(getColvalue>comp_value )
				{
					SetColWidth(2, 75);
					SetRowHeight(fcount, 20);
					SetSH_Width(0);
					SetTH_Height(20);
			
					//AfxMessageBox(L"Hide Row");
				}
				else
				{
					SetColWidth(2, 75);
					SetRowHeight(fcount, 0);
					SetSH_Width(0);
					SetTH_Height(20);
					//AfxMessageBox(L"UN Hide Row");
				}
			}
			if (fcar==L"<")
			{
				if(getColvalue<comp_value )
				{
					SetColWidth(2, 75);
					SetRowHeight(fcount, 20);
					SetSH_Width(0);
					SetTH_Height(20);
			
					//AfxMessageBox(L"Hide Row");
				}
				else
				{
					SetColWidth(2, 75);
					SetRowHeight(fcount, 0);
					SetSH_Width(0);
					SetTH_Height(20);
					//AfxMessageBox(L"UN Hide Row");
				}
			}
		}

		else
		{

		if(getColvalue==col_value || col_value==L"ALL")
		{
			SetColWidth(2, 75);
			SetRowHeight(fcount, 20);
			SetSH_Width(0);
			SetTH_Height(20);
			
			//AfxMessageBox(L"Hide Row");
		}
		else
		{
			SetColWidth(2, 75);
			SetRowHeight(fcount, 0);
			SetSH_Width(0);
			SetTH_Height(20);
			//AfxMessageBox(L"UN Hide Row");
		}
		}
	}
	m_logfile_g.LogEvent(L"End gridFilter");
}

int GridTradeAndOrder::OnCanViewMove(int oldcol,long oldrow,int newcol,long newrow)
{
	m_logfile_g.LogEvent(L"Start OnCanViewMove");
	UNREFERENCED_PARAMETER(oldcol);
	UNREFERENCED_PARAMETER(oldrow);
	UNREFERENCED_PARAMETER(newcol);
	UNREFERENCED_PARAMETER(newrow);
	m_logfile_g.LogEvent(L"End  OnCanViewMove");
	return TRUE;
}