
// PathFinderView.h : interface of the CPathFinderView class
//

#pragma once


class CPathFinderView : public CListView
{
protected: // create from serialization only
	CPathFinderView() noexcept;
	DECLARE_DYNCREATE(CPathFinderView)

// Attributes
public:
	CPathFinderDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CPathFinderView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStarttrace();
	afx_msg void OnUpdateStarttrace(CCmdUI* pCmdUI);
	afx_msg void OnStoptrace();
	afx_msg void OnUpdateStoptrace(CCmdUI* pCmdUI);
	afx_msg void OnRetrace();
	afx_msg void OnUpdateRetrace(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveAs();
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnFileConfig();

	LRESULT InsertNormalRow(WPARAM wParam, LPARAM lParam);
	LRESULT InserTimeoutRow(WPARAM wParam, LPARAM lParam);
	LRESULT AddRowInfo(WPARAM wParam, LPARAM lParam);
	LRESULT AddNodeName(WPARAM wParam, LPARAM lParam);
};

#ifndef _DEBUG  // debug version in PathFinderView.cpp
inline CPathFinderDoc* CPathFinderView::GetDocument() const
   { return reinterpret_cast<CPathFinderDoc*>(m_pDocument); }
#endif

