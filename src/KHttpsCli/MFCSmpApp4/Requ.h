#pragma once
class KRequ
{
public:
	KRequ(PWS usrID = nullptr);
	//CString _dmn{ L"localhost" };//
	CString _dmn{ L"www.parent.co.kr" };//localhost
	CString _url{ L"http%s://%s:%d/api?func=%s&uuid=%s&noCache=%s" };

	CString _UsrID;//�α����� ��� �ʼ�
	CString _BizID;
	//CString _bizID;//�α����� ��� �ʼ�
	//CString _fUsrID; // update�� ���. �α��� �� ����� ������
	bool _noCache{ false };//false�̸� ������.
	bool _ssl{ false };
	int _port{ 19479 };
	bool _errBox{ true };
	// L"http://v2.petme.kr:19479/api?func=ReadSomedata&uuid=PetMeSMP?noCache=true"
	CString getUrl(PAS sfunc)
	{
		CString url;
		url.Format(_url, _ssl ? L"s" : L"", _dmn, _port, CStringW(sfunc), _UsrID, _noCache ? L"true" : L"false");
		return url;
	}

	void RequestPost(PAS sfunc, JObj& jin, JObj& jout);
	void RequestGet(PAS sfunc, JObj& jin, JObj& jout) {}
};

