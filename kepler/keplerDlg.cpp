// keplerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "kepler.h"
#include "keplerDlg.h"
#include <math.h>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const double M=1.98E30;			////////Масса Солнца ////////////
const double G=6.67E-11;		///Гравитационная постоянная////
const double DisplayScaleX = 1e9;
const double DisplayScaleVX = 1000;
//const double g=9.87e4;	
///Ускорение своб. падения//////

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CkeplerDlg dialog

CkeplerDlg::CkeplerDlg(CWnd* pParent /*=NULL*/)
: CDialog(CkeplerDlg::IDD, pParent)
,xkm(149E6)
,ykm(0)
,vxkm(0)
,vykm(34)
,dt(86400)
,ntimeout(100)
,xwin(0)
,ywin(0)
,vxwin(0)
,vywin(0)
,m(1)
,a(0)
,b(0)
,eks(0)
,at(0)
,n(0)
,scale(1e6)// километров занимает пиксель на экране
,period(0)
,kepler3(0)
,eDeviation(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	points = new std::vector<RKresult>();
	energy = new std::vector<Energy>();
	cycleIsFound = false;
}

void CkeplerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_xkm, xkm);
	DDX_Text(pDX, IDC_ykm, ykm);
	DDX_Text(pDX, IDC_vxkm, vxkm);
	DDX_Text(pDX, IDC_vykm, vykm);
	DDX_Text(pDX, IDC_xwin, xwin);
	DDX_Text(pDX, IDC_ywin, ywin);
	DDX_Text(pDX, IDC_vxwin, vxwin);
	DDX_Text(pDX, IDC_vywin, vywin);
	DDX_Text(pDX, IDC_a, a);
	DDX_Text(pDX, IDC_b, b);
	DDX_Text(pDX, IDC_eks, eks);
	DDX_Text(pDX, IDC_T, period);
	DDX_Text(pDX, IDC_Kepler3, kepler3);
	DDX_Text(pDX, IDC_eMed, eDeviation);
	DDX_Text(pDX, IDC_dt, dt);
	DDX_Text(pDX, IDC_eFullNow, eFullNow);
	DDX_Text(pDX, IDC_ePotentialNow, ePotentialNow);
	DDX_Text(pDX, IDC_eCineticNow, eCineticNow);
	DDX_Text(pDX, IDC_eFullMax, eFullMax);
	DDX_Text(pDX, IDC_ePotentialMax, ePotentialMax);
	DDX_Text(pDX, IDC_eCineticMax, eCineticMax);
}

BEGIN_MESSAGE_MAP(CkeplerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER() //это вписал я
	ON_BN_CLICKED(IDC_START, &CkeplerDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CkeplerDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_BUTTON1, &CkeplerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CkeplerDlg::OnBnClickedButton2)
	//ON_BN_CLICKED(IDC_CLEANER, &CkeplerDlg::OnBnClickedClean)
END_MESSAGE_MAP()


// CkeplerDlg message handlers

BOOL CkeplerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	UpdateData(TRUE);
	// TODO: Add extra initialization here
	frameWnd = GetDlgItem(IDC_EnergyG);
	frameDce = frameWnd->GetDC();
	frameWnd->GetClientRect(&rectE);
	frameWnd = GetDlgItem(IDC_TRA);
	frameDc = frameWnd->GetDC();

	frameWnd->GetClientRect(&rect);

//	frameWnd = GetDlgItem(IDC_U_GRAF);
	//UDc = frameWnd->GetDC();
	//frameWnd->GetClientRect(&Urect);
	UpdateData(TRUE);
	RSolar = 30; // визуальный радиус Солнца 
	xpi = xkm * 1000;
	ypi = ykm * 1000;
	RealTime=0;
//	//// начальные условия ///
	xt=xpi+RSolar;
	yt=ypi;
	cycleIsFound = false;
	//VxShip=V0*V0;
	//VyShip=0;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CkeplerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CkeplerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
		frameDc->Rectangle(rect.left+10,
			rect.top+20,
			rect.right-10,
			rect.bottom-10);
			DrawGrafic();

	}
}

void CkeplerDlg::DrawGrafic()
{
	double scale = 1e9;// метров занимает пиксель на экране
	int RSolar=30;
	int RS=5;// пиксели
	xCenter=rect.Width()/2;
	yCenter=rect.Height()/2;

	brush.CreateSolidBrush(RGB(255, 255, 0));// цвет Земли(255,255,0)
	oldbrush = frameDc->SelectObject(&brush);

	frameDc->Ellipse(xCenter-RSolar,yCenter+RSolar,xCenter+RSolar,	yCenter-RSolar);
	frameDc->MoveTo(0,yCenter);
	frameDc->LineTo(rect.Width(),yCenter);
	frameDc->MoveTo(xCenter,0);
	frameDc->LineTo(xCenter, rect.Height());

	frameDc->SelectObject(oldbrush);
	brush.DeleteObject();
	// строим спутник
brush.CreateSolidBrush(RGB(0, 0, 180));// цвет Земли(0,0,180)
	oldbrush = frameDc->SelectObject(&brush);

	int xs = xt / scale;
	int ys = yt / scale;

	frameDc->Ellipse(
		xCenter+(int)(xs)-RS,
		yCenter-(int)(ys)-RS,
		xCenter+(int)(xs)+RS,
		yCenter-(int)(ys)+RS);
	for (int i = 0; i < points->size(); ++i) {
		RKresult pt = points->at(i);
		frameDc->SetPixel(xCenter + (int)(pt.x/scale), yCenter - (int)(pt.y / scale), RGB(20, 0, 200));
	}

	frameDc->SelectObject(oldbrush);

	double upper = 1.2e9;
	double lower = -1.2e9;
	/*
	if (cycleIsFound) {
		double upper = energy->at(0).cinetic;
		double lower = energy->at(0).potential;
		for (int i = 0; i < energy->size(); ++i) {
			Energy ecp = energy->at(i);
			upper = max(upper, max(max(ecp.cinetic, ecp.full), ecp.potential));
			lower = min(lower, min(min(ecp.cinetic, ecp.full), ecp.potential));
		}
		double fx = (upper - lower) * 0.2;
		lower -= fx;
		upper += fx;*/
		double rg = upper - lower;
		COLORREF cineticColor = RGB(200, 0, 0);
		COLORREF potentialColor = RGB(20, 0, 200);
		COLORREF fullColor = RGB(20, 240, 0);
		frameDce->SetTextColor(cineticColor);
		frameDce->TextOutW(5, 16, CString("Кинетическая"));
		frameDce->SetTextColor(potentialColor);
		frameDce->TextOutW(5, 48, CString("Потенциальная"));
		frameDce->SetTextColor(fullColor);
		frameDce->TextOutW(5, 80, CString("Полная"));
		for (long hValue = -800000000; hValue <= 800000000; hValue += 400000000) {
			int h = (upper - hValue) / rg * rectE.Height();
			frameDce->MoveTo(15, h);
			frameDce->LineTo(rect.Width() - 15, h);
			
			frameDce->SetTextColor(cineticColor);
			frameDce->TextOutW(rect.Width() - 380, h - 8 , CString(std::to_string(hValue).c_str()));
		}
		for (int i = 0; i < energy->size(); ++i) {
			Energy ecp = energy->at(i);
			rectE.Height();
			int y = (upper - ecp.cinetic) / rg * rectE.Height();
			frameDce->SetPixel(i, y, cineticColor);
			y = (upper - ecp.potential) / rg * rectE.Height();
			frameDce->SetPixel(i, y, potentialColor);
			y = (upper - ecp.full) / rg * rectE.Height();
			frameDce->SetPixel(i, y, fullColor);
		}
	//}

	brush.DeleteObject();
	//траектория
    pen.CreatePen(0,1,RGB(255,0,0));
		oldpen=frameDc->SelectObject(&pen);
		int x1 = xCenter + int(xkm * 1000) + RSolar;
		int y1 = yCenter + int(ykm * 1000);
		int imax=(int)(x1-xt)/2;
		//at=par_a(100.00,xt,yt,vxt,vyt);
		/*frameDc->MoveTo((int)x1,(int)y1);
		for (int i=1;i<=(int)imax;i+=2)
		{
				frameDc->LineTo(xCenter+(int)x1+2*i ,yCenter+(int)y1-2*i );
				frameDc->MoveTo(xCenter+(int)x1-4*i ,yCenter+(int)y1+4*i );

		}*/
		frameDc->SelectObject(oldpen);
			pen.DeleteObject();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CkeplerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CkeplerDlg::OnBnClickedStart()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	xpi = xkm * 1000;
	ypi = ykm * 1000;
	// начальные условия 

	xt=xpi+RSolar;
	yt=ypi;
	vyt = vykm * 1000;
	vxt = vxkm * 1000;
	InvalidateRect(&rect);
	//frameDc->TextOutW(20,20,_T("Старт"));
	nTimer=SetTimer(ID_TIMER1,ntimeout,NULL);
	cycleIsFound = false;
	a = 0;
	b = 0;
	period = 0;
	points->clear();
	energy->clear();
	eDeviation = 0;
	kepler3 = 0;

	eCineticMax = -1e20;
	ePotentialMax = -1e20;
	eFullMax = -1e20;
	
	frameDce->FillSolidRect(CRect(rectE.left, rectE.top + 16, rectE.right, rectE.bottom), frameDce->GetBkColor());

}


double GetEPot(RKresult& point)
{
	return -G * M / sqrt(point.x * point.x + point.y * point.y);
}

double GetECinetic(RKresult& point)
{
	return (point.Vx * point.Vx + point.Vy * point.Vy) / 2;
}

void CkeplerDlg::OnTimer(UINT nIDEvent)
{
	SendParameters();
		RKresult Koor=RKMethod(RealTime,xt,yt,vxt,vyt);
		double step = sqrt((xt-Koor.x)*(xt - Koor.x) + (yt - Koor.y)* (yt - Koor.y));
			xt=Koor.x;
		yt=Koor.y;
		vxt=Koor.Vx;
		vyt=Koor.Vy;
		Energy e = Energy(GetEPot(Koor), GetECinetic(Koor));
		eCineticNow = e.cinetic;
		ePotentialNow = e.potential;
		eFullNow = e.full;
		eCineticMax = max(eCineticMax, eCineticNow);
		ePotentialMax = max(ePotentialMax, ePotentialNow);
		eFullMax = max(eFullMax, eFullNow);
		if (!cycleIsFound) {
			int index = points->size();
			points->push_back(Koor);
			energy->push_back(Energy(GetEPot(points->at(index)), GetECinetic(points->at(index))));
			if (points->size() > 20) {
				double lengthToStart = sqrt((xt - points->at(0).x) * (xt - points->at(0).x) + (yt - points->at(0).y) * (yt - points->at(0).y));
				if (lengthToStart < step) {
					// found a cycle!!
					cycleIsFound = true;
					n = points->size();
					int i = 0;
					int n2 = n / 2;
					for (i = 0; i < n2; ++i) {
						double dx = points->at(i).x - points->at(i + n2).x;
						double dy = points->at(i).y - points->at(i + n2).y;

						a = max(sqrt(dx * dx + dy * dy), a);
					}
					b = a;
					for (i = 0; i < n2; ++i) {
						double dx = points->at(i).x - points->at(i + n2).x;
						double dy = points->at(i).y - points->at(i + n2).y;

						b = min(sqrt(dx * dx + dy * dy), b);
					}
					double eMed = 0;
					for (i = 0; i < n; i++) eMed = eMed + GetECinetic(points->at(i)) + GetEPot(points->at(i));
					eMed /= n;
					double eDeviationMax = 0.0;
					for (i = 0; i < n; i++) {
						double eDeviation = fabs((-eMed + GetECinetic(points->at(i)) + GetEPot(points->at(i)))/eMed);
						eDeviationMax = max(eDeviationMax, eDeviation);
					}
					this->eks = sqrt(1.0 - b*b/a/a);
					eDeviation = eDeviationMax;
					period = n * dt/86400;
					kepler3 = n * dt * n * dt / a / a / a;
				}
			}
		}
		RealTime=Koor.t;
		InvalidateRect(&rect);
}

void CkeplerDlg::OnBnClickedStop()
{
	KillTimer(nTimer);
	// TODO: Add your control notification handler code here
}
void CkeplerDlg::SendParameters()
{
	xwin=xt/ DisplayScaleX;//*m;
	ywin=yt/ DisplayScaleX;//*m;
	vxwin=vxt/ DisplayScaleVX;
	vywin=vyt/ DisplayScaleVX;
	UpdateData(FALSE);
}

//////////////////	Метод Рунге-Кутта ////////////////
double CkeplerDlg::f(double t,double x,double y,double vx,double vy)
{
	return -G*M/(x*x+y*y)*(x/sqrt(x*x+y*y));
}

double CkeplerDlg::g(double t,double x,double y,double vx,double vy)
{
	return -G*M/(x*x+y*y)*(y/sqrt(x*x+y*y));
}

RKresult CkeplerDlg::RKMethod(double t,double x,double y,double vx,double vy)
{
	double k1x,k1y,k2x,k2y,k3x,k3y,k4x,k4y;
	
	k1x=f(t,x,y,vx,vy)*dt;
	k1y=g(t,x,y,vx,vy)*dt;
	k2x=f(t+dt/2, x+vx*dt/2, y+vy*dt/2, vx+k1x/2, vy+k1y/2)*dt;
	k2y=g(t+dt/2, x+vx*dt/2, y+vy*dt/2, vx+k1x/2, vy+k1y/2)*dt;
	k3x=f(t+dt/2, x+vx*dt/2+k1x/4*dt, y+vy*dt/2+k1y/4*dt, vx+k2x/2, vy+k2y/2)*dt;
	k3y=g(t+dt/2, x+vx*dt/2+k1x/4*dt, y+vy*dt/2+k1y/4*dt, vx+k2x/2, vy+k2y/2)*dt;
	k4x=f(t+dt, x+vx*dt+k2x*dt/2, y+vy*dt+k2y*dt/2, vx+k3x, vy+k3y)*dt;
	k4y=g(t+dt, x+vx*dt+k2x*dt/2, y+vy*dt+k2y*dt/2, vx+k3x, vy+k3y)*dt;

	double tNew=t+dt;
	double xNew=x+vx*dt+((k1x+k2x+k3x)/6)*dt;
	double yNew=y+vy*dt+((k1y+k2y+k3y)/6)*dt;
	double vxNew=vx+(k1x+2*k2x+2*k3x+k4x)/6;
	double vyNew=vy+(k1y+2*k2y+2*k3y+k4y)/6;

	RKresult koor4(tNew,xNew,yNew,vxNew,vyNew);
	return koor4;
}
/*double CkeplerDlg::par_a(double t,double x,double y,double vx,double vy)
{
	int RSolar=(int)(6400/m);
	xCenter=rect.Width()/2;
	yCenter=rect.Height()/2;

	RKresult Koorfor_a=RKMethod(t,x,y,vx,vy);
	double x_a;
	if(Koorfor_a.Vx==0 && Koorfor_a.x!=(xCenter+(int)xkm/m+RSolar)){
		x_a=abs(Koorfor_a.x);
	// break;}
	double a4;
	a4=(x_a+xCenter+(int)xkm/m+RSolar)/2;
	return a4;

}*/


void CkeplerDlg::OnBnClickedButton1()

{
	UpdateData(TRUE);
	xpi=xkm/m;
	vykm=sqrt(sqrt(G*M/(xpi+RSolar)));
	UpdateData(FALSE);
	// TODO: Add your control notification handler code here
}

void CkeplerDlg::OnBnClickedButton2()
{
	UpdateData(TRUE);
	a=xkm/m;
	b=(xkm/m);
	eks=sqrt(1-(b*b)/(a*a));
	UpdateData(FALSE);
	// TODO: Add your control notification handler code here
}
