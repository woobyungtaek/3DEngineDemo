#pragma once

///--------------------------------------------------
/// Animation Data�� �����ϱ� ���� Ŭ����
///--------------------------------------------------

class CAnimation_pos
{
public:
	CAnimation_pos() :m_time(0) { };

public:
	int		m_time;
	Vector3	m_pos;
};

class CAnimation_rot
{
public:
	CAnimation_rot() : m_time(0), m_angle(0.0f) {};

public:
	int				m_time;
	Vector3			m_rot;
	float			m_angle;
	Quaternion		m_rotQT_accumulation;		// ��뺯���̱� ������ ���� ����� �����ؼ� ������ �ִ´� (�� ���ݱ����� ���뷮���� ��ȯ�Ѵ�)
};

class CAnimation_scl
{
public:
	CAnimation_scl() : m_time(0), m_scaleaxisang(0.0f) {};
public:
	int			m_time;
	Vector3		m_scale;
	Vector3		m_scaleaxis;
	float		m_scaleaxisang;
	Quaternion	m_scalerotQT_accumulation;
};


///--------------------------------------------------
/// �� Ŭ������ ������ Animation Ŭ����
///--------------------------------------------------
class Animation
{
public:
	Animation() : m_ticksperFrame(0), m_minKeyframe(0) {};
	~Animation() {	};
public:
	// �ִϸ��̼� �������� �̸�. ������ ���������̴�
	std::string					m_nodename;
	vector<CAnimation_pos*>		m_position;
	vector<CAnimation_rot*>		m_rotation;
	vector<CAnimation_scl*>		m_scale;

	int		m_ticksperFrame;	// �� �������� ƽ ũ��. �̰��� AnimationTable�� ������ ������ ������Ʈ���� �ٸ� �� �ִ�.
	int		m_minKeyframe;		// Ű �����Ӱ��� �ּ�ũ��. �̰��� ���ǰͰ� �� ��ġ������ �ʴ´�.
};