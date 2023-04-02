// keplerDlg.h : header file
//

#pragma once
#define ID_TIMER1 1000

struct RKresult
{
	double t;
	double x;
	double y;
	double Vx;	
	double Vy;
	RKresult(double t_, double x_, double y_, double Vx_, double Vy_)
	{
		t=t_;
		x=x_;
		y=y_;		
		Vx=Vx_;
		Vy=Vy_;
	};
};

struct Energy
{
	double potential;
	double cinetic;
	double full;
	Energy(double _potential, double _cinetic)
	{
		potential	= _potential;
		cinetic		= _cinetic;
		full		= potential + cinetic;
	};
};


// CkeplerDlg dialog
class CkeplerDlg : public CDialog
{
// Construction
public:
	CkeplerDlg(CWnd* pParent = NULL);	
	// standard constructor
CWnd *frameWnd;

	CRect frameRect,
		rect,
		Urect;
	CRect rectE;

	CDC* UDc;		// для граф-ка потенциальной энергии
	//CDC* TDc;		// для граф-ка кинетической энергии
	//CDC* EDc;		// для граф-ка полной энергии

	CDC *frameDc;	// для анимации спутника
	CDC *frameDce;	// для графика энергии



	CDC *pMemDc;		// контекст рисования в памяти компьютера
	CBitmap bmp, *tmp;	// изображение-растр

	CBrush brush,
		*oldbrush; //создаем кисть
	CPen pen,
		*oldpen; //создаем перо

	int xCenter, // центр по Х
		yCenter, // центр по Y
		nTimer,
		ntimeout, //масштабные множетели
		RSolar,
		m;
	int n;
	double xt,yt,vxt,vyt,xpi,ypi,dt,RealTime;
	double scale;// километров занимает пиксель на экране

// Dialog Data
	enum { IDD = IDD_KEPLER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


private:
	std::vector<RKresult>* points;
	std::vector<Energy>* energy;
	bool cycleIsFound;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void DrawGrafic();
	afx_msg void DrawTRA(CDC *pDC,CRect r,double xt,double yt);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	double xkm;
	double ykm;
	double vxkm;
	double vykm;

public:
	double xwin;
	double ywin;
	double vxwin;
	double vywin;
	double period;
	double eDeviation;
	double kepler3;
	double eFullNow;
	double ePotentialNow;
	double eCineticNow;
	double eFullMax;
	double ePotentialMax;
	double eCineticMax;


//public:
//	afx_msg void OnBnClickedClean();
public:
	afx_msg void OnBnClickedStart();
public:
	afx_msg void OnBnClickedStop();
	afx_msg void OnTimer(UINT nIDEvent);
	void SendParameters();

	double f(double t,double x,double y,double vx,double vy);
	double g(double t,double x,double y,double vx,double vy);
	RKresult RKMethod(double t,double x,double y,double vx,double vy);
	/*double par_a(double t,double x,double y,double vx,double vy);
	double par_b(double t,double x,double y,double vx,double vy);
	double par_eks(double t,double x,double y,double vx,double vy);*/
public:
	afx_msg void OnBnClickedButton1();
public:
	double a;
	double at;
public:
	double b;
public:
	double eks;
public:
	afx_msg void OnBnClickedButton2();
};
