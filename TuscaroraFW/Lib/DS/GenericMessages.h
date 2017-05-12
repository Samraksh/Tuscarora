////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef GENERIC_MESSAGE_H
#define GENERIC_MESSAGE_H


#include <iostream>
#include <type_traits>
#include <assert.h>
#include <cstdint>
#include <cstring>
#include <stdlib.h>
#include <Lib/Misc/Debug.h>

#include <sys/socket.h>
#include <Sys/SocketShim.h>
#include <unistd.h>

#include <sys/ioctl.h>

#define DBG_GEN_MSG 0
typedef std::size_t GenericMsgPayloadSize_t;

template< class T >
struct TypeIsInt
{
    static const bool value = false;
};

template<>
struct TypeIsInt< GenericMsgPayloadSize_t >
{
    static const bool value = true;
};


template<typename T1, typename T2>
T2* AllocatePointerSpace(GenericMsgPayloadSize_t i1, T2* i2){
	i2 = (T2*)malloc(i1);
	return i2;
}
template<typename T1, typename T2>
T2 AllocatePointerSpace(T1 i1, T2 i2){
	 assert(0);
	 return i2;
}


class Generic_VarSized_Msg{
protected:
    GenericMsgPayloadSize_t  payloadSize;
    uint8_t* payload;
    mutable uint8_t* position;
    GenericMsgPayloadSize_t byteswritten;
    mutable GenericMsgPayloadSize_t bytesread;



public:
    Generic_VarSized_Msg (GenericMsgPayloadSize_t  _payload_size){
        payload = new uint8_t[_payload_size];
        position = payload;
        payloadSize = _payload_size;
        byteswritten = 0;
        bytesread = 0;
        //size = payload_size + CAL_HEADER_SIZE;
    };
    Generic_VarSized_Msg(GenericMsgPayloadSize_t  _payload_size, uint8_t* _payload){
        payloadSize = _payload_size;
        payload = _payload;
        position = 0;
        byteswritten = _payload_size;
        bytesread = 0;
    };
    ~Generic_VarSized_Msg () {
        if (DBG_GEN_MSG) {
            std::cout << "  Destructing  ~Generic_VarSized_Msg " << " payloadSize = " << payloadSize;
        }
        delete[] payload;

    };
    void RewindPosition(){
        position = payload;
    }

    inline uint8_t* GetPayload() const{
        return payload;
    }
    inline GenericMsgPayloadSize_t  GetPayloadSize() const{
        return payloadSize;
    }
    inline GenericMsgPayloadSize_t GetBytesRead() const{
        return bytesread;
    }

    template<typename T>
    inline void WriteToPayload (const T& data){
        GenericMsgPayloadSize_t bytes2bewritten = sizeof(T);
        if (DBG_GEN_MSG) {
            std::cout << "WriteToPayload1 entered  " << '\n';
        }
        if (DBG_GEN_MSG) {
            std::cout << "  byteswrittenbefore =  " << byteswritten << " bytes2bewritten = " << bytes2bewritten << " payloadSize = " << payloadSize;
        }
        if (position == NULL) position = GetPayload();
        assert ( payload != NULL);
        assert ( position  >=  payload);
        assert ( position  <= payload + payloadSize);
        assert ( position + bytes2bewritten <= payload + payloadSize);
        std::size_t bytes2bewritten2 = bytes2bewritten;
        memcpy( position , &data, bytes2bewritten2);
        position = position + bytes2bewritten;
        byteswritten += bytes2bewritten;
        if (DBG_GEN_MSG) {
            std::cout << "  byteswrittenafter =  " << byteswritten<< " ";
        }
        if (DBG_GEN_MSG) {
            std::cout << '\n';
        }
    }
    template<typename T1, typename T2>
    inline void WriteToPayload (GenericMsgPayloadSize_t i1, T2 i2){
        if(!std::is_pointer<T2>::value){
            if (DBG_GEN_MSG) {
                std::cout << "WriteToPayload4 entered  " << '\n';
            }
            assert(0);
        }
        else{
        if (DBG_GEN_MSG) {
            std::cout << "WriteToPayload3 entered  " << '\n';
        }
        if (DBG_GEN_MSG) {
            std::cout << "  byteswrittenbefore =  " << byteswritten << " bytes2bewritten = " << i1 << " payloadSize = " << payloadSize;
        }
        if (position == NULL) position = GetPayload();
        assert ( payload != NULL);
        assert ( i2 != NULL);
        assert ( position  >=  payload);
        assert ( position  <= payload + payloadSize);
        assert ( position + i1 <= payload + payloadSize);
        std::size_t bytes2bewritten2 = i1;
        memcpy( (void*)position , (const void*)i2, bytes2bewritten2);
        byteswritten += i1;
        position = position + i1;
        if (DBG_GEN_MSG) {
            std::cout << "  byteswrittenafter =  " << byteswritten << " ";
        }

        if (DBG_GEN_MSG) {
            std::cout << '\n';
        }
        }
    }
    template<typename T1, typename T2>
    inline void WriteToPayload (T1 i1, T2 i2){
        if (DBG_GEN_MSG) {
            std::cout << "WriteToPayload2 entered  " << '\n';
        }
        assert(0);
    }



    template<typename T>
    inline void ReadFromPayload (T& data) const{
        GenericMsgPayloadSize_t bytes2beread = sizeof(T);
        if (DBG_GEN_MSG) {
            std::cout << "ReadFromPayload1 entered  " << '\n';
        }
        if (DBG_GEN_MSG) {
            std::cout << "  bytesreadbefore =  " << bytesread << " bytes2beread = " << bytes2beread << " payloadSize = " << payloadSize << "\n";
        }
        if (position == NULL) {
            position = GetPayload();
        }
        assert ( payload != NULL);
        assert ( position  >=  payload);
        assert ( position  <= payload + payloadSize);
        assert ( position + bytes2beread <= payload + payloadSize);
        std::size_t bytes2beread2 = bytes2beread;
//        void* dataread = malloc(bytes2beread2);
//        memcpy(dataread, position, bytes2beread2);
//        data = *((T*)dataread);
        memcpy( &data, position, bytes2beread2);
        position = position + bytes2beread;
        bytesread += bytes2beread;
        if (DBG_GEN_MSG) {
            std::cout << "  bytesreadafter =  " << bytesread<< " ";
        }
        if (DBG_GEN_MSG) {
            std::cout << '\n';
        }
    }
    template<typename T1, typename T2>
    inline void ReadFromPayload (GenericMsgPayloadSize_t i1, T2& i2) const{
        if(!std::is_pointer<T2>::value){
            if (DBG_GEN_MSG) {
                std::cout << "ReadFromPayload3 entered  " << '\n';
            }
            assert(0);
        }
        else{
			if (DBG_GEN_MSG) {
				std::cout << "ReadFromPayload3 entered  " << '\n';
			}
			if (DBG_GEN_MSG) {
				std::cout << "  bytesreadbefore =  " << bytesread << " bytes2bewread = " << i1 << " payloadSize = " << payloadSize;
			}
			if (DBG_GEN_MSG) {
				std::cout << "ReadFromPayload3 entered  " << '\n';
			}

			if (i2 == NULL) {
				//i2 = new T2[i1];
				//i2 = (T2)malloc(i1);
				i2 = AllocatePointerSpace<T1,T2>(i1, i2);
			}
			assert ( payload != NULL);
			assert ( i2 != NULL);
			assert ( position  >=  payload);
			assert ( position  <= payload + payloadSize);
			assert ( position + i1 <= payload + payloadSize);
			std::size_t bytes2beread2 = i1;
			memcpy( (void*)i2, (const void*)position, bytes2beread2);
			bytesread += i1;
			position = position + i1;
			if (DBG_GEN_MSG) {
				std::cout << "  bytesreadafter =  " << bytesread << " ";
			}

			if (DBG_GEN_MSG) {
				std::cout << '\n';
			}
        }
    }
    template<typename T1, typename T2>
    inline void ReadFromPayload (T1 i1, T2 i2) const{
        if (DBG_GEN_MSG) {
            std::cout << "ReadFromPayload2 entered  " << '\n';
        }
        assert(0);
    }



    inline void CopyToPayload (void* _payload) {
        memcpy( payload, _payload, payloadSize);
        byteswritten = payloadSize;
    }
};

// BK: This is a work in progress for Serializer. It mainly works but needs :
// Changing the size and the pointer order
// Careful implementation of deserializer
template<typename T1,typename T2, typename... TNs>
class GenericSerializer{
public:
    Generic_VarSized_Msg* msg;
    GenericMsgPayloadSize_t payloadSize;


    template<typename C1, typename C2>
    void Add2PayloadSize(const C1 av){
        if (DBG_GEN_MSG) {
            std::cout << "Add2PayloadSize2 entered  ";
        }

        if(TypeIsInt< C1 >::value){
            if (DBG_GEN_MSG) {
                std::cout << "Add2PayloadSize2 "<< " is type integer = " << TypeIsInt< C1 >::value << "  is pointer=" <<std::is_pointer<C1>::value << '\n';
            }
            //payloadSize += av;
        }
        else{
            if (DBG_GEN_MSG) {
                std::cout << "Add2PayloadSize2 cannot process because type is not integer"<<" is type integer = " << TypeIsInt< C1 >::value << "  is pointer=" <<std::is_pointer<C1>::value << '\n';
            }

        }
    };
    template<typename C1, typename C2>
    void Add2PayloadSize(GenericMsgPayloadSize_t av){
        if (DBG_GEN_MSG) {
            std::cout << "Add2PayloadSize3 entered  ";
        }

        if(TypeIsInt< C1 >::value){
            if (DBG_GEN_MSG) {
                std::cout << "Add2PayloadSize3 "<< " is type integer = " << TypeIsInt< C1 >::value << "  is pointer=" <<std::is_pointer<C1>::value << '\n';
            }

            payloadSize += av;

            //if (DBG_GEN_MSG) {
            std::cout << "av = "<< av <<" sizeof(C2) =  " << sizeof(C2) << " total = "<< av * sizeof(C2) <<"  payloadSize = " << payloadSize <<'\n';
        }
        else{
        if (DBG_GEN_MSG) {
            std::cout << "Add2PayloadSize3 cannot process because type is not integer"<<" is type integer = " << TypeIsInt< C1 >::value << "  is pointer=" <<std::is_pointer<C1>::value << '\n';
        }

        }
};
template<typename C1>
void Add2PayloadSize(GenericMsgPayloadSize_t av){
    if (DBG_GEN_MSG) {
        std::cout << "Add2PayloadSize1 entered  ";
    }

    if (DBG_GEN_MSG) {
        std::cout << "Add2PayloadSize1 "<< " is type integer = " << TypeIsInt< GenericMsgPayloadSize_t >::value << "  is pointer=" <<std::is_pointer<GenericMsgPayloadSize_t>::value << '\n';
    }
    payloadSize += av ;

};

template<typename C1>
void CalculateSize(const C1& i1){
    if (DBG_GEN_MSG) {
        std::cout << "CalculateSize with 1 input entered ";
    }
    if(!std::is_pointer<C1>::value){
        if (DBG_GEN_MSG) {
            std::cout << "CalculateSize processing non-ptr "<< " is type integer = " << TypeIsInt< C1 >::value << "  is pointer=" <<std::is_pointer<C1>::value << '\n';
        }
    }
    GenericMsgPayloadSize_t a = (GenericMsgPayloadSize_t)sizeof(C1);
    Add2PayloadSize<GenericMsgPayloadSize_t>(a);
}

template<typename C1, typename C2>
void CalculateSize(const C1& i1, const C2& i2){
    if (DBG_GEN_MSG) {
        std::cout << "CalculateSize with 2 input entered  ";
    }
    if(!std::is_pointer<C2>::value) {
        CalculateSize<C2>(i2);
    }
    else {
        assert(!std::is_pointer<C1>::value);
        if (DBG_GEN_MSG) {
            std::cout << "CalculateSize processing pointer "<< "is type integer = " << TypeIsInt< C1 >::value<< "  is pointer=" <<std::is_pointer<C2>::value << '\n';
        }
        Add2PayloadSize<C1,C2>(i1);
    }
}
template<typename C1, typename C2,typename C3, typename... CNs>
void CalculateSize(const C1& i1, const C2& i2, const C3& i3, const CNs&... ins){
    if (DBG_GEN_MSG) {
        std::cout << "CalculateSize with 3 input entered  ";
    }
    if(!std::is_pointer<C2>::value) {
        CalculateSize<C2>(i2);
    }
    else {
        assert(!std::is_pointer<C1>::value);
        if (DBG_GEN_MSG) {
            std::cout << "CalculateSize processing pointer "<< "is type integer = " << TypeIsInt< C1 >::value<< "  is pointer=" <<std::is_pointer<C2>::value << '\n';
        }
        Add2PayloadSize<C1,C2>(i1);
    }
    CalculateSize<C2, C3, CNs...>(i2, i3, ins...);
}

template<typename C1>
void AddVariable(const C1& i1){
    if (DBG_GEN_MSG) {
        std::cout << "AddVariable with 1 input entered "<< " sizeof(C1)= "<<sizeof(C1) << "  ";
    }
    if(!std::is_pointer<C1>::value){
        if (DBG_GEN_MSG) {
            std::cout << "AddVariable with 1 input "<< " is type integer = " << TypeIsInt< C1 >::value << "  is pointer=" <<std::is_pointer<C1>::value << '\n';
        }
        msg->WriteToPayload<C1>(i1);
    }
}
template<typename C1, typename C2>
void AddVariable(const C1& i1, const C2& i2){
    if (DBG_GEN_MSG) {
        std::cout << "AddVariable with 2 input entered "<< " sizeof(C2) "<<sizeof(C2)<< "  ";
    }
    if(!std::is_pointer<C2>::value) AddVariable<C2>(i2);
    else {
        if (DBG_GEN_MSG) {
            std::cout << "AddVariable with 2 input "<< "is type integer = " << TypeIsInt< C2 >::value<< "  is pointer=" <<std::is_pointer<C2>::value << '\n';
        }
        assert(std::is_pointer<C1>::value == 0);
        assert(std::is_pointer<C2>::value == 1);
        // void *p = static_cast<void*>(i2);
        msg->WriteToPayload<C1,C2>(i1, i2);
    }
}
template<typename C1, typename C2, typename C3,typename... CNs>
void AddVariable(const C1& i1, const C2& i2, const C3& i3, const CNs&... ins){
    if (DBG_GEN_MSG) {
        std::cout << "AddVariable with 3+ input entered "<< " sizeof(C2) "<<sizeof(C2)<< "  ";
    }
    if(!std::is_pointer<C2>::value) AddVariable<C2>(i2);
    else {
        if (DBG_GEN_MSG) {
            std::cout << "AddVariable with 3+ input "<< "is type integer = " << TypeIsInt< C2 >::value<< "  is pointer=" <<std::is_pointer<C2>::value << " sizeof(C2) "<<sizeof(C2) <<'\n';
        }
        assert(std::is_pointer<C1>::value == 0);
        assert(std::is_pointer<C2>::value == 1);
        // void *p = static_cast<void*>(i2);
        msg->WriteToPayload<C1,C2>(i1, i2);
    }
    AddVariable<C2, C3, CNs...>(i2, i3, ins...);
}


GenericSerializer(const T1& i1, const T2& i2, const TNs&... ins){
    payloadSize = 0;
    this->CalculateSize<T1>(i1);
    this->CalculateSize<T1, T2, TNs...>(i1, i2, ins...);
    if (DBG_GEN_MSG) {
        std::cout << "  payloadSize = " << payloadSize <<'\n';
    }

    if (DBG_GEN_MSG) {
        std::cout << "\n\n";
    }


    msg = new Generic_VarSized_Msg(payloadSize);
    this->AddVariable(i1);
    this->AddVariable(i1, i2, ins...);

};
/*GenericSerializer(T1& i1){
        payloadSize = 0;
        this->CalculateSize(i1);
        msg = new Generic_VarSized_Msg(payloadSize);
        this->AddVariable(i1);
    };*/
/*GenericSerializer(){
        msg = NULL;
        payloadSize = 0;
    };*/
Generic_VarSized_Msg* Get_Generic_VarSized_Msg_Ptr(){
    return msg;
};

virtual ~GenericSerializer(){
	delete msg;
}

};

/*
 * GenericDeSerializer:This copies serial_msg contents to the variables. For the case of pointers, it copies contents to the location pointed by the pointer. If the pointer is NULL, then malloc is called to allocate memory,
 */
template<typename T1,typename T2=void*, typename... TNs>
class GenericDeSerializer{
    Generic_VarSized_Msg const* msg;
    //GenericMsgPayloadSize_t payloadSize;
    template<typename C1>
    void GetVariable(C1& i1){
        if (DBG_GEN_MSG) {
            std::cout << "GetVariable with 1 input entered "<< " sizeof(C1)= "<<sizeof(C1) << "  ";
        }
        if(!std::is_pointer<C1>::value){
            if (DBG_GEN_MSG) {
                std::cout << "GetVariable with 1 input "<< " is type integer = " << TypeIsInt< C1 >::value << "  is pointer=" <<std::is_pointer<C1>::value << '\n';
            }
            msg->ReadFromPayload<C1>(i1);
        }
    }
    template<typename C1, typename C2>
    void GetVariable(C1& i1, C2& i2){
        if (DBG_GEN_MSG) {
            std::cout << "GetVariable with 2 input entered "<< " sizeof(C2) "<<sizeof(C2)<< "  ";
        }
        if(!std::is_pointer<C2>::value) GetVariable<C2>(i2);
        else {
            if (DBG_GEN_MSG) {
                std::cout << "GetVariable with 2 input "<< "is type integer = " << TypeIsInt< C2 >::value<< "  is pointer=" <<std::is_pointer<C2>::value << '\n';
            }
            assert(std::is_pointer<C1>::value == 0);
            assert(std::is_pointer<C2>::value == 1);
            msg->ReadFromPayload<C1,C2>(i1, i2);
        }
    }
    template<typename C1, typename C2, typename C3,typename... CNs>
    void GetVariable(C1& i1, C2& i2, C3& i3, CNs&... ins){
        if (DBG_GEN_MSG) {
            std::cout << "GetVariable with 3+ input entered "<< " sizeof(C2) "<<sizeof(C2)<< "  ";
        }
        if(!std::is_pointer<C2>::value) GetVariable<C2>(i2);
        else {
            if (DBG_GEN_MSG) {
                std::cout << "GetVariable with 3+ input "<< "is type integer = " << TypeIsInt< C2 >::value<< "  is pointer=" <<std::is_pointer<C2>::value << " sizeof(C2) "<<sizeof(C2) <<'\n';
            }
            assert(std::is_pointer<C1>::value == 0);
            assert(std::is_pointer<C2>::value == 1);
            msg->ReadFromPayload<C1,C2>(i1,i2);
        }
        GetVariable<C2, C3, CNs...>(i2, i3, ins...);
    }


public:
    /*
     * GenericDeSerializer:This copies serial_msg contents to the variables.
     * For the case of pointers, it copies contents to the location pointed by the pointer.
     * If the pointer is NULL, then malloc is called to allocate memory and the pointer is changed to point to the new location.
     *
     */
    GenericDeSerializer(Generic_VarSized_Msg const * const _msg, T1& i1, T2& i2, TNs&... ins){
        msg = _msg;
        //payloadSize = msg->GetPayloadSize();
        this->GetVariable(i1);
        this->GetVariable(i1, i2, ins...);
    };
    GenericDeSerializer(Generic_VarSized_Msg const * const  _msg, T1& i1){
        msg = _msg;
        //payloadSize = msg->GetPayloadSize();
        this->GetVariable(i1);
    };

};




/*
 * GenericDeSerializer:This copies serial_msg contents to the variables. For the case of pointers, it copies contents to the location pointed by the pointer. If the pointer is NULL, then malloc is called to allocate memory,
 */
class SocketDeSerializer{
	GenericMsgPayloadSize_t bytesread;
	mutable uint32_t num_vars_read;
    GenericMsgPayloadSize_t payloadSize;
    bool blockingread;

    template<typename C1, typename C2>
    void Add2PayloadSize(const C1 av){
        if (DBG_GEN_MSG) {
            std::cout << "Add2PayloadSize2 entered  ";
        }

        if(TypeIsInt< C1 >::value){
            if (DBG_GEN_MSG) {
                std::cout << "Add2PayloadSize2 "<< " is type integer = " << TypeIsInt< C1 >::value << "  is pointer=" <<std::is_pointer<C1>::value << '\n';
            }
            //payloadSize += av;
        }
        else{
            if (DBG_GEN_MSG) {
                std::cout << "Add2PayloadSize2 cannot process because type is not integer"<<" is type integer = " << TypeIsInt< C1 >::value << "  is pointer=" <<std::is_pointer<C1>::value << '\n';
            }

        }
    };
    template<typename C1, typename C2>
    void Add2PayloadSize(GenericMsgPayloadSize_t av){
        if (DBG_GEN_MSG) {
            std::cout << "Add2PayloadSize3 entered  ";
        }

        if(TypeIsInt< C1 >::value){
            if (DBG_GEN_MSG) {
                std::cout << "Add2PayloadSize3 "<< " is type integer = " << TypeIsInt< C1 >::value << "  is pointer=" <<std::is_pointer<C1>::value << '\n';
            }

            payloadSize += av;

            //if (DBG_GEN_MSG) {
            std::cout << "av = "<< av <<" sizeof(C2) =  " << sizeof(C2) << " total = "<< av * sizeof(C2) <<"  payloadSize = " << payloadSize <<'\n';
        }
        else{
        if (DBG_GEN_MSG) {
            std::cout << "Add2PayloadSize3 cannot process because type is not integer"<<" is type integer = " << TypeIsInt< C1 >::value << "  is pointer=" <<std::is_pointer<C1>::value << '\n';
        }

        }
};
	template<typename C1>
	void Add2PayloadSize(GenericMsgPayloadSize_t av){
		if (DBG_GEN_MSG) {
			std::cout << "Add2PayloadSize1 entered  ";
		}

		if (DBG_GEN_MSG) {
			std::cout << "Add2PayloadSize1 "<< " is type integer = " << TypeIsInt< GenericMsgPayloadSize_t >::value << "  is pointer=" <<std::is_pointer<GenericMsgPayloadSize_t>::value << '\n';
		}
		payloadSize += av ;

	};

	template<typename C1>
	void CalculateSize(const C1& i1){
		if (DBG_GEN_MSG) {
			std::cout << "CalculateSize with 1 input entered ";
		}
		if(!std::is_pointer<C1>::value){
			if (DBG_GEN_MSG) {
				std::cout << "CalculateSize processing non-ptr "<< " is type integer = " << TypeIsInt< C1 >::value << "  is pointer=" <<std::is_pointer<C1>::value << '\n';
			}
		}
		GenericMsgPayloadSize_t a = (GenericMsgPayloadSize_t)sizeof(C1);
		Add2PayloadSize<GenericMsgPayloadSize_t>(a);
	}
	template<typename C1, typename C2>
	void CalculateSize(const C1& i1, const C2& i2){
		if (DBG_GEN_MSG) {
			std::cout << "CalculateSize with 2 input entered  ";
		}
		if(!std::is_pointer<C2>::value) {
			CalculateSize<C2>(i2);
		}
		else {
			assert(!std::is_pointer<C1>::value);
			if (DBG_GEN_MSG) {
				std::cout << "CalculateSize processing pointer "<< "is type integer = " << TypeIsInt< C1 >::value<< "  is pointer=" <<std::is_pointer<C2>::value << '\n';
			}
			Add2PayloadSize<C1,C2>(i1);
		}
	}
	template<typename C1, typename C2,typename C3, typename... CNs>
	void CalculateSize(const C1& i1, const C2& i2, const C3& i3, const CNs&... ins){
		if (DBG_GEN_MSG) {
			std::cout << "CalculateSize with 3 input entered  ";
		}
		if(!std::is_pointer<C2>::value) {
			CalculateSize<C2>(i2);
		}
		else {
			assert(!std::is_pointer<C1>::value);
			if (DBG_GEN_MSG) {
				std::cout << "CalculateSize processing pointer "<< "is type integer = " << TypeIsInt< C1 >::value<< "  is pointer=" <<std::is_pointer<C2>::value << '\n';
			}
			Add2PayloadSize<C1,C2>(i1);
		}
		CalculateSize<C2, C3, CNs...>(i2, i3, ins...);
	}


    template<typename T>
    inline bool ReadFromSocket (T& data) const{
        if (DBG_GEN_MSG) {
            std::cout << "ReadFromSocket1 entered  " << '\n';
        }
        if(!std::is_pointer<T>::value){
        	return RecvBytesfromSocket(&data, sizeof(T));
        }
        else{
        	return true; // TODO: Think about this case.
        }
    }
    template<typename T2>
    inline bool ReadFromSocket (std::size_t i1, T2& i2) const{
        if(!std::is_pointer<T2>::value){
            if (DBG_GEN_MSG) {
                std::cout << "ReadFromSocket2 entered with a no pointer second input  " << '\n';
            }
            assert(0);
            return false;
        }
        else{
			if (DBG_GEN_MSG) {
				std::cout << "ReadFromSocket2 entered  " << '\n';
			}


			if (i2 == NULL) {
//				i2 = (T2)malloc(i1);
				i2 = AllocatePointerSpace(i1,i2);
			}
			if(i2 == NULL){
				std::cout << "ReadFromSocket: Allocation unsuccessfull " << '\n';
			}
			assert ( i2 != NULL);


		  return RecvBytesfromSocket(i2, i1);

        }
    }
    template<typename T1, typename T2>
    inline bool ReadFromSocket (T1 i1, T2 i2) const{
        if (DBG_GEN_MSG) {
            std::cout << "ReadFromSocket3 entered  " << '\n';
        }
        assert(0);
        return false;
    }



    //GenericMsgPayloadSize_t payloadSize;
    template<typename C1>
    bool GetVariable(C1& i1){
        if (DBG_GEN_MSG) {
            std::cout << "GetVariable with 1 input entered "<< " sizeof(C1)= "<<sizeof(C1) << "  ";
        }
        if(!std::is_pointer<C1>::value){
            if (DBG_GEN_MSG) {
                std::cout << "GetVariable with 1 input "<< " is type integer = " << TypeIsInt< C1 >::value << "  is pointer=" <<std::is_pointer<C1>::value << '\n';
            }
            return this->ReadFromSocket<C1>(i1);
        }
    }
    template<typename C1, typename C2>
    bool GetVariable(C1& i1, C2& i2){
        if (DBG_GEN_MSG) {
            std::cout << "GetVariable with 2 input entered "<< " sizeof(C2) "<<sizeof(C2)<< "  ";
        }
        if(!std::is_pointer<C2>::value) {
        	return this->GetVariable<C2>(i2);
        }
        else {
            if (DBG_GEN_MSG) {
                std::cout << "GetVariable with 2 input "<< "is type integer = " << TypeIsInt< C2 >::value<< "  is pointer=" <<std::is_pointer<C2>::value << '\n';
            }
            assert(std::is_pointer<C1>::value == 0);
            assert(std::is_pointer<C2>::value == 1);
            return this->ReadFromSocket(i1, i2);
        }
    }
    template<typename C1, typename C2, typename C3,typename... CNs>
    bool GetVariable(C1& i1, C2& i2, C3& i3, CNs&... ins){
    	bool cont;
        if (DBG_GEN_MSG) {
            std::cout << "GetVariable with 3+ input entered "<< " sizeof(C2) "<<sizeof(C2)<< "  ";
        }
        if(!std::is_pointer<C2>::value) {
        	cont = GetVariable<C2>(i2);
        }
        else {
            if (DBG_GEN_MSG) {
                std::cout << "GetVariable with 3+ input "<< "is type integer = " << TypeIsInt< C2 >::value<< "  is pointer=" <<std::is_pointer<C2>::value << " sizeof(C2) "<<sizeof(C2) <<'\n';
            }
            assert(std::is_pointer<C1>::value == 0);
            assert(std::is_pointer<C2>::value == 1);
            cont = this->ReadFromSocket(i1,i2);
        }
        if(!cont) return false;
        else return GetVariable<C2, C3, CNs...>(i2, i3, ins...);
    }


    int32_t socketID;
public:

    uint32_t GetNumVarsRead(){ return num_vars_read;}


    /*
     * GenericDeSerializer:This copies serial_msg contents to the variables.
     * For the case of pointers, it copies contents to the location pointed by the pointer.
     * If the pointer is NULL, then malloc is called to allocate memory and the pointer is changed to point to the new location.
     *
     */
    template<typename T>
    bool RecvBytesfromSocket(T i1, uint32_t size, int flags = 0) const{
    	return false;
    }
    template<typename T>
    bool RecvBytesfromSocket(T* i1, uint32_t size, int flags = 0) const{
    	void* buf = (void*)i1;
    	//First check if the entire data for the variable is available
    	ssize_t newlyreadbytes = 0;
    	ssize_t readBytes = 0;
    	char* tip_of_buffer = (char*) buf;
//    	int32_t readBytes = recv(socketID,buf, size, MSG_PEEK ); //First check whether the entire data length is available in the socket
    	while(readBytes < size){
    		newlyreadbytes = recv(socketID, tip_of_buffer, size, flags );
    		if(newlyreadbytes == 0) { //Case for closed connections
    			return false;
    		}
    		else if(newlyreadbytes < 0 ) { //Case for unsuccessful reads
    			if(!blockingread){
    				return false;
    			}
    		}
    		else{
    			readBytes += newlyreadbytes;
    			tip_of_buffer += readBytes;
    		}
    	}
    	++num_vars_read;
    	return true;
    }
    //  SocketDeSerializer( SocketReaderFnc _func){
    SocketDeSerializer( const int32_t _socketID, bool _blockingread = true){
    	socketID = _socketID;
    	blockingread = _blockingread;
    }
    template<typename T1,typename T2=void*, typename... TNs>
    bool Read(T1& i1, T2& i2, TNs&... ins){
    	bool rv;
    	if(!blockingread){
    		//    	//First Check whether all data was received

						//Calculate total size and check whether that much data is available at the socket
			payloadSize = 0;
			this->CalculateSize<T1>(i1);
			this->CalculateSize<T1, T2, TNs...>(i1, i2, ins...);
					//    		//Perform  a dry run
//			ioctl(fd,FIONREAD,&bytes_available)
			int32_t readBytes = 0;
			void* dummy_read = malloc(payloadSize);
			readBytes = recv(socketID, dummy_read, payloadSize, MSG_PEEK);
			free(dummy_read);
			if(readBytes < (int32_t)payloadSize) { //If all variables are not ready to be read
				return false;
			}
    	}

    	rv = false;
    	num_vars_read = 0;
    	printf("SocketDeSerializer reading on %d : ", socketID); fflush(stdout);
        if(this->GetVariable(i1))
        	rv = this->GetVariable(i1, i2, ins...);
        printf("\nSocketDeSerializer END OF reading on %d : rv = %d ", socketID, rv); fflush(stdout);
        return rv;
    };
    template<typename T1>
    bool Read(T1& i1){
    	bool rv = false;
    	printf("SocketDeSerializer reading on %d : ", socketID); fflush(stdout);
        rv = this->GetVariable(i1);
        printf("\nSocketDeSerializer END OF reading on %d : rv = %d ", socketID, rv); fflush(stdout);
        return rv;
    };
    template<typename C1>
    bool Peek(C1& i1){
        if(!std::is_pointer<C1>::value){
            if (DBG_GEN_MSG) {
                std::cout << "GetVariable with 1 input "<< " is type integer = " << TypeIsInt< C1 >::value << "  is pointer=" <<std::is_pointer<C1>::value << '\n';
            }
            return RecvBytesfromSocket(&i1, sizeof(C1), MSG_PEEK );
        }
        return false;
    };

};

/*
 * SocketSerializer: This module writes the contents of the variables one by one to a socket */
// BK: This is a work in progress for Serializer. It mainly works but needs :
// Changing the size and the pointer order
// Careful implementation of deserializer


class SocketSerializer{

    template<typename T>
    inline bool WriteToSocket (const T& data, bool isMoreVars){
        if (DBG_GEN_MSG) {
            std::cout << "WriteToSocket1 entered  " << '\n';
        }
        return WriteBytesToSocket(&data, sizeof(T), isMoreVars);
    }
    template< typename T2>
    inline bool WriteToSocket (std::size_t i1, T2* i2, bool isMoreVars){
		if (DBG_GEN_MSG) {
			std::cout << "WriteToSocket2 entered  " << '\n';
		}
		assert ( i2 != NULL);
		return WriteBytesToSocket(i2, i1, isMoreVars);
    }
    template<typename T1, typename T2>
    inline bool WriteToSocket (T1 i1, T2 i2, bool isMoreVars){
        if (DBG_GEN_MSG) {
            std::cout << "WriteToSocket3 entered  " << '\n';
        }
        assert(0);
    }

	template<typename C1>
	bool AddVariable(const C1& i1, bool isMoreVars = false){
		if (DBG_GEN_MSG) {
			std::cout << "AddVariable with 1 input entered "<< " sizeof(C1)= "<<sizeof(C1) << "  ";
		}
		if(!std::is_pointer<C1>::value){
			if (DBG_GEN_MSG) {
				std::cout << "AddVariable with 1 input "<< " is type integer = " << TypeIsInt< C1 >::value << "  is pointer=" <<std::is_pointer<C1>::value << '\n';
			}
			return WriteToSocket<C1>(i1,isMoreVars);
		}
		return false;
	}
	template<typename C1, typename C2>
	bool AddVariable(const C1& i1, const C2& i2){
		if (DBG_GEN_MSG) {
			std::cout << "AddVariable with 2 input entered "<< " sizeof(C2) "<<sizeof(C2)<< "  ";
		}
		if(!std::is_pointer<C2>::value) {
			return AddVariable<C2>(i2);
		}
		else {
			if (DBG_GEN_MSG) {
				std::cout << "AddVariable with 2 input "<< "is type integer = " << TypeIsInt< C2 >::value<< "  is pointer=" <<std::is_pointer<C2>::value << '\n';
			}
			assert(std::is_pointer<C1>::value == 0);
			assert(std::is_pointer<C2>::value == 1);
			// void *p = static_cast<void*>(i2);
			return WriteToSocket(i1, i2, false);
		}
	}
	template<typename C1, typename C2, typename C3,typename... CNs>
	bool AddVariable(const C1& i1, const C2& i2, const C3& i3, const CNs&... ins){
		bool cont;
		if (DBG_GEN_MSG) {
			std::cout << "AddVariable with 3+ input entered "<< " sizeof(C2) "<<sizeof(C2)<< "  ";
		}
		if(!std::is_pointer<C2>::value) {
			cont = AddVariable<C2>(i2);
		}
		else {
			if (DBG_GEN_MSG) {
				std::cout << "AddVariable with 3+ input "<< "is type integer = " << TypeIsInt< C2 >::value<< "  is pointer=" <<std::is_pointer<C2>::value << " sizeof(C2) "<<sizeof(C2) <<'\n';
			}
			assert(std::is_pointer<C1>::value == 0);
			assert(std::is_pointer<C2>::value == 1);
			// void *p = static_cast<void*>(i2);
			cont = WriteToSocket(i1, i2, true);
		}
		if(!cont) return false;
		else return AddVariable<C2, C3, CNs...>(i2, i3, ins...);
	}

	int32_t socketID;
    bool WriteBytesToSocket(const void * buf, const uint32_t size, bool isMoreVars = true) const{
    	//size_t readBytes = write((int)socketID, buf, (size_t)size);
    	size_t sentBytes;
		if(isMoreVars){
			sentBytes = send((int)socketID, buf, (size_t)size, MSG_MORE);
		}
		else{
			sentBytes = send((int)socketID, buf, (size_t)size, 0);
		}


    	char* cbuf = (char*)buf;
		for (uint i=0; i< size; i++){
			printf("%X ", cbuf[i]); fflush(stdout);
		}

    	return sentBytes>0;
    }

public:
SocketSerializer( int32_t _socketID){
	socketID = _socketID;
}

template<typename T1,typename T2=void*, typename... TNs>
bool Write(const T1& i1, const T2& i2, const TNs&... ins){
	 bool rv = false;
	 printf("SocketSerializer writing on %d : ", socketID); fflush(stdout);

	 if(this->AddVariable(i1, true))
	 	 rv =  this->AddVariable(i1, i2, ins...);
	 printf("\n SocketSerializer END OF writing on %d : rv = %d ", socketID, rv); fflush(stdout);
	 return rv;
};

template<typename T1>
bool Write(T1& i1){
	bool rv = false;
	printf("SocketSerializer writing on %d : ", socketID); fflush(stdout);

    rv  = this->AddVariable(i1);
    printf("\n SocketSerializer END OF writing on %d : rv = %d", socketID, rv); fflush(stdout);
    return rv;
};

};

#endif // GENERIC_MESSAGE_H
