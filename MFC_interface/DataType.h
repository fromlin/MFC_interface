//////////////////////////////////////////////////////////////////////////////////////////////////////////
// File		: DataType.h
// Version	: 1.0.1
// Date		: 2022.08.31
// Writer	: Kim, YeLin (MRAS)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#define DEG2RAD						0.0174533
#define RAD2DEG						57.2958

#define INV_LINEAR_RESOLUTION		1000./0.006 	// [mm]
#define INV_ANGULAR_RESOLUTION		1./0.09			// [deg]

#define GA							-9.80665

typedef struct _omegaData {

	double pos[3];
	double ori[3];
	double grp;

}OmegaData_t;

typedef struct _sendPacketData {

	unsigned char header[4];
	unsigned char size, id;
	unsigned char mode, check;

	int pos[3];
	int ori[3];
	int grp;	

}SendPacketData_t;

typedef union _sendPacket {

	SendPacketData_t data;
	unsigned char buffer[sizeof(SendPacketData_t)];

}SendPacket_t;





typedef struct _stmData {

	double fdb[3];
	
}StmData_t;

typedef struct _recvPacketData {

	unsigned char header[4];
	unsigned char size, id;
	unsigned char mode, check;
	
	int fdb[3];

}RecvPacketData_t;

typedef union _recvPacket {

	RecvPacketData_t data;
	unsigned char buffer[sizeof(RecvPacketData_t)];

}RecvPacket_t;





typedef struct _kukaData {

	double pos[3];
	double ori[3];

	double force[3];
	double torq[3];

	double jpos[7];
	double jtorq[7];

}KuKaData_t;





typedef struct _canData {

	double force[3];
	double torq[3];

}CanData_t;





typedef struct _touchData {

	double pos[3];
	double ori[3];

	int state;

}TouchData_t;





typedef union _artecData {

	struct {
		float x;
		float y;
		float z;
	};
	float data[3];

}ArtecData_t, *PArtecData_t;



/////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <array>

template <typename Type, size_t Count>
class SimpleRingBuffer {
public:
	SimpleRingBuffer() { flush(); }

	constexpr size_t count() const { return Count; }

	SimpleRingBuffer& push(Type value) {
		m_buffer[m_head] = value;
		m_head++;
		if (m_head == Count) {
			m_head = 0;
		}
		return *this;
	}

	Type back() const { return m_buffer[m_head]; }
	Type front() const { return at(0); }

	Type at(size_t offset) {
		const auto adjusted_offset
			= m_head > offset ? m_head - offset - 1 : Count + m_head - offset - 1;
		return m_buffer[adjusted_offset];
	}

	SimpleRingBuffer& flush() {
		for (auto& value : m_buffer) {
			value = {};
		}
		m_head = 0;
		return *this;
	}

private:
	size_t m_head = 0;
	std::array<Type, Count> m_buffer;
};

template <typename Type, size_t Count>
class SimpleMovingAverage {
public:
	constexpr size_t count() const { return Count; }

	SimpleMovingAverage& calculate(Type input) {
		sum += input;
		sum -= m_buffer.back();
		m_buffer.push(input);
		return *this;
	}

	Type present_value() const { return sum / Count; }

private:
	SimpleRingBuffer<Type, Count> m_buffer;
	Type sum = {};
};