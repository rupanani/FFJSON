/* 
 * File:   FFJSON.h
 * Author: Satya Gowtham Kudupudi
 *
 * Created on November 29, 2013, 4:29 PM
 */

#ifndef FFJSON_H
#define FFJSON_H

#define MAX_ORDERED_MEMBERS 1000
#define MAX_MEM_ITER_UPDATE 100
#include <logger.h>
#include <ferrybase/FerryTimeStamp.h>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <exception>
#include <list>
#include <set>
#include <stdint.h>
#include <cstring>

using namespace std;

enum FFJ_LOG {
    FFJ_MAIN
};


class FFJSON {
public:

	enum OBJ_TYPE : uint8_t {
		UNDEFINED,
		STRING,
		XML,
		NUMBER,
		BOOL,
		OBJECT,
		ARRAY,
		LINK,
		BINARY,
		BIG_OBJECT,
		TIME,
		NUL
	};

	enum QUERY_TYPE : uint32_t {
		/**
		 * To clear query type
		 */
		NONE = 0 << 8,
		QUERY = 1 << 8,
		SET = 2 << 8,
		DELETE = 3 << 8,
		UPDATE = 4 << 8
	};

	enum E_FLAGS : uint32_t {
		ENONE = 0,
		B64ENCODE = 1 << 16,
		B64ENCODE_CHILDREN = 1 << 17,
		B64ENCODE_STOP = 1 << 18,
		COMMENT = 1 << 19,
		HAS_COMMENT = 1 << 20,

		EXTENDED = 1 << 21, //ARRAY N OBJECT //FM
		LONG_LAST_LN = 1 << 21, //STRING //NO_FM

		PRECISION = 1 << 22, //NUMBER //FM
		EXT_VIA_PARENT = 1 << 22, //ARRAY N OBJECT //FM
		ONE_SHORT_LAST_LN = 1 << 22, //STRING //NO_FM

		HAS_CHILDREN = 1 << 23, //ARRAY N OBJECT //FM
		STRING_INIT = 1 << 23 //STRING //FM
	};

	enum COPY_FLAGS : uint32_t {
		COPY_NONE = 0,
		COPY_QUERIES = 1 << 0,
		COPY_EFLAGS = 1 << 1,
		COPY_ALL = 0xffffffff
	};

	enum FeaturedMemType : uint32_t {
		FM_TABHEAD = 1,
		FM_PRECISION = 1,
		FM_WIDTH = 1,
		FM_LINK = 2,
		FM_PARENT = 3,
		FM_CHILDREN = 4,
		FM_MAP_SEQUENCE = 5,
		FM_MULTI_LN = 6,
        FM_UPDATE_TIMESTAMP = 7
	};

	class Exception : exception {
	public:

		Exception(string e) : identifier(e) {

		}

		const char* what() const throw () {
			return this->identifier.c_str();
		}

		~Exception() throw () {
		}
	private:
		string identifier;
	};

	class Iterator {
	public:
		Iterator();
		Iterator(const Iterator& orig);
		Iterator(const FFJSON& orig, bool end = false);
		Iterator(map<string, FFJSON*>::iterator pi);
		Iterator(vector<FFJSON*>::iterator ai);
        Iterator(vector<map<string, FFJSON*>::iterator >::iterator pai, vector<map<string,FFJSON*>::iterator>* pMapItVec);
		virtual ~Iterator();
		void init(const FFJSON& orig, bool end = false);
		Iterator& operator++();
		Iterator operator++(int);
		Iterator& operator--();
		Iterator operator--(int);
		Iterator& operator=(const Iterator& i);
		Iterator& operator+(int i);
		bool operator==(const Iterator& i);
		bool operator!=(const Iterator& i);
		FFJSON* operator->();
		FFJSON& operator*();
		operator const char*();
		/**
		 * Should be only used on OBJECT type iterators
		 * @return name in the name-value pair of the iterator of the OBJECT
		 */
		string GetIndex();
		/**
		 * Should be only use on ARRAY type iterators
		 * @param rCurrArray should be the FFJSON Object of the iterator
		 * @return index of the iterator of the ARRAY
		 */
		int GetIndex(const FFJSON& rCurrArray);

    private:
        uint8_t type;
		void copy(const Iterator& i);

        union IteratorUnion {
            map<string, FFJSON*>::iterator pi;
            vector<FFJSON*>::iterator ai;
            vector<map<string, FFJSON*>::iterator >::iterator pai;

            IteratorUnion() {
                memset(this, 0, sizeof (IteratorUnion));
            }

            IteratorUnion(const IteratorUnion& rFIt) {
                memcpy(this, &rFIt, sizeof (IteratorUnion));
            }

            IteratorUnion(IteratorUnion&& rFIt) {
                memcpy(this, &rFIt, sizeof (IteratorUnion));
            }

            ~IteratorUnion() {
            }

            IteratorUnion(const vector<map<string, FFJSON*>::iterator >::iterator&
                    itMapVector) {
                pai = itMapVector;
            }

            IteratorUnion(const vector<FFJSON*>::iterator& itVec) {
                ai = itVec;
            }

            IteratorUnion(const map<string, FFJSON*>::iterator& itMap) {
                pi = itMap;
            }

            IteratorUnion& operator=(const IteratorUnion& rFIt) {
                memcpy(this, &rFIt, sizeof (IteratorUnion));
                return *this;
            }

            IteratorUnion& operator=(IteratorUnion&& rFIt) {
                memcpy(this, &rFIt, sizeof (IteratorUnion));
                return *this;
            }

            friend bool operator<(const IteratorUnion& lhs, const IteratorUnion&
                    rhs) {
                return true;
            }
        } ui;
        union ContainerPs{
            map<string, FFJSON*>* m_pMap;
            vector<FFJSON*>* m_pVector;
            vector<map<string, FFJSON*>::iterator>* m_pMapVector;
        } m_uContainerPs;
	};

	struct FeaturedMemHook;
	typedef vector<string> Link;

	union FeaturedMember {
		Link* link;
		map<string, int>* tabHead;
		FFJSON* m_pParent;
		/**
		 * used to store the number precision
		 */
		unsigned int precision = 0;
		/**
		 * used to store the width of the string
		 */
		unsigned int width;
		/**
		 * used to link another Featured member
		 */
		FeaturedMemHook* m_pFMH;
		/**
		 * used for multiline buffer while parsing
		 */
		string* m_psMultiLnBuffer;
		/**
		 * used to mark a multi line array during init
		 */
		bool m_bIsMultiLineArray;
		/**
		 * array of links of all children. these links must be deleted up on
		 * change.
		 */
		vector<FFJSON*>* m_pvChildren;
		/**
		 * Its a vector of names in a map for the order
		 */
		vector<map<string, FFJSON*>::iterator>* m_pvpsMapSequence;

		FeaturedMember() : link{
			NULL
		}
		{
		}
        FerryTimeStamp* m_pTimeStamp;
	};

	struct FeaturedMemHook {

		FeaturedMemHook() {
			m_uFM.m_pFMH = NULL;
			m_pFMH.m_pFMH = NULL;
		}
		FeaturedMember m_uFM;
		FeaturedMember m_pFMH;
	};

	struct FFJSONExt {
		FFJSON* base = NULL;
	};

    struct FFJSONPObj {
        const string* name;
		FFJSON* value = NULL;
        FFJSONPObj* pObj = NULL;
        vector<map<string, FFJSON*>::iterator>* m_pvpsMapSequence;
	};

	struct FFJSONPrettyPrintPObj : FFJSONPObj {
		FFJSONPrettyPrintPObj(map<const string*, const string*>* m_mpDeps,
				list<string>* m_lsFFPairLst,
				map<string*, const string*>* m_mpMemKeyFFPairMap,
				map<const string*, list<string>::iterator>*
				pKeyPrettyStringMap);
		bool m_bHeaded = false;

		/**
		 * to get the parent of object
		 */
		map<const string*, const string*>* m_mpDeps = NULL;
		list<string>* m_lsFFPairLst = NULL;
		map<string*, const string*>* m_mpMemKeyFFPairMap = NULL;
		map<const string*, list<string>::iterator>*
				m_pKeyPrettyStringItMap = NULL;

		/**
		 * Holds column widths for tabular members
		 */
		map<string, vector<int> >* m_msviClWidths = NULL;

		/**
		 * If this flag is set returns 1st line of string
		 */
		bool m_bGiveFirstLine = false;
	};

	union FFJSONIterator {
		map<string, FFJSON*>::iterator m_itMap;
		unsigned int m_uiIndex;

		FFJSONIterator() {
			memset(this, 0, sizeof (FFJSONIterator));
		}

		FFJSONIterator(const FFJSONIterator& rFIt) {
			memcpy(this, &rFIt, sizeof (FFJSONIterator));
		}

		FFJSONIterator(FFJSONIterator&& rFIt) {
			memcpy(this, &rFIt, sizeof (FFJSONIterator));
		}

		~FFJSONIterator() {
		}

		FFJSONIterator(const map<string, FFJSON*>::iterator& itMap) {
			m_itMap = itMap;
		}

		FFJSONIterator(const unsigned int uiIndex) {
			m_uiIndex = uiIndex;
		}

		FFJSONIterator& operator=(const FFJSONIterator& rFIt) {
			memcpy(this, &rFIt, sizeof (FFJSONIterator));
			return *this;
		}

		FFJSONIterator& operator=(FFJSONIterator&& rFIt) {
			memcpy(this, &rFIt, sizeof (FFJSONIterator));
			return *this;
		}

		friend bool operator<(const FFJSONIterator& lhs, const FFJSONIterator&
				rhs) {
			return true;
		}
	};

	struct LinkNRef {
		FFJSON* m_pRef=0;
		string m_sLink;
	};

	union FFValue {
		char * string;
		vector<FFJSON*>* array;
		map<std::string, FFJSON*>* pairs;
		double number;
		bool boolean;
		FFJSON* fptr;
		void* vptr;
		FerryTimeStamp* m_pFerryTimeStamp;
		FFValue() : string{
			NULL
		}
		{
		}
	} val;

	/**
	 * creates an UNRECOGNIZED FFJSON object. Any FFJSON object can be
	 * assigned any other type of FFJSON object.
	 */
	FFJSON();

	enum COPY_FLAGS : uint32_t;
	class FFJSONPObj;
	/**
	 * Copy constructor. Creates a copy of FFJSON object
	 * @param orig is the object one wants to create a copy
	 */
	FFJSON(const FFJSON& orig, COPY_FLAGS cf = COPY_ALL,
			FFJSONPObj* pObj = NULL);

	/**
	 * Creates a FFJSON object from a FFJSON string.
	 * @param ffjson is the FFJSON string to be parsed.
	 * @param ci is the offset in FFJSON string to be considered. Its 0 by 
	 * default.
	 */
	FFJSON(const string& ffjson, int* ci = NULL, int indent = 0,
			FFJSONPObj* pObj = NULL);
	void init(const string& ffjson, int* ci = NULL, int indent = 0,
			FFJSONPObj* pObj = NULL);

	/**
	 * Creates an empty FFJSON object of type @param t. It throws an Exception
	 * if @param t is UNRECOGNIZED or anything else other FFJSON_OBJ_TYPE
	 * @param t
	 */
	FFJSON(OBJ_TYPE t);

	~FFJSON();
	/**
	 * Emptys the FFJSON object. For example If you want delete objects in an 
	 * array or an object, invoke it.
	 */
	void freeObj(bool bAssignment=false);

	static const FeaturedMemType m_FM_LAST = FM_PARENT;
	static const char OBJ_STR[10][15];
	static map<string, uint8_t> STR_OBJ;
	static FFJSON* MarkAsUpdatable(string link, const FFJSON& rParent);
	static FFJSON* UnMarkUpdatable(string link, const FFJSON& rParent);

	void insertFeaturedMember(FeaturedMember& fms, FeaturedMemType fMT);
	FeaturedMember getFeaturedMember(FeaturedMemType fMT) const;
	void destroyAllFeaturedMembers(bool bExemptQueries=false);
	void deleteFeaturedMember(FeaturedMemType fmt);

	/**
	 * It holds the size of the FFJSON object. array size, object properties, string
	 * length. Do not change it!! Its made public only for reading convenience.
	 */
	unsigned int size = 0;

	/**
	 * If the object is of type @param t, it returns true else false.
	 * @param t : type to check
	 * @return true if type matched
	 */
	bool isType(OBJ_TYPE t) const;

	/**
	 * Sets type of the object to t
	 * @param t
	 */
	void setType(OBJ_TYPE t);

	OBJ_TYPE getType() const;

	bool isQType(QUERY_TYPE t) const;

	void setQType(QUERY_TYPE t);

	QUERY_TYPE getQType() const;

	bool isEFlagSet(E_FLAGS t) const;

	void setEFlag(E_FLAGS t);

	E_FLAGS getEFlags() const;

	void clearEFlag(E_FLAGS t);

	void setFMCount(uint32_t iFMCount);

	/**
	 * Removes leading and trailing white spaces; sapces and tabs from a string.
	 * @param s
	 */
	static void trimWhites(string& s);

	/**
	 * Removes leading and trailing quotes in a string.
	 * @param s
	 */
	static void trimQuotes(string& s);

	/**
	 * Trying to read an object property that doesn't exist creates the property
	 * with unrecognized object.
	 * @param f
	 */
	void trim();

	/**
	 * Gives FFJSON object type of FFJSON string.
	 * @param ffjson is the FFJSON string.
	 * @return FFJSON object type.
	 */
	OBJ_TYPE objectType(string ffjson);

	/**
	 * Converts FFJSON object into FFJSON string.
	 * @return FFJSON string.
	 */
	string stringify(bool json = false, bool GetQueryStr = false,
			FFJSONPrettyPrintPObj* pObj = NULL)const;
	
	#define GetQueryString(...) stringify(false,true,NULL);

	/**
	 * Converts FFJSON object into FFJSON pretty string that has indents where
	 * they needed
	 * @param indent : Dont bother about it! Its 0 by default which you need. If
	 * you insist, it prepends its value number of indents to the output. To get
	 * an idea on what I'm saying, jst try it with non zero positive value.
	 * @return A pretty string :)
	 */
	string prettyString(bool json = false, bool printComments = false,
			unsigned int indent = 0, FFJSONPrettyPrintPObj* pObj = NULL) const;

	/**
	 * Generates a query string which can be used to query a FFJSON tree. Query 
	 * string is constructed based on SET, QUERY and DELETE marks on the FFJSON
	 * objects.
	 * E.g.
	 * {animals:{horses:{count:?,colors:?}}} can be used to query horses count 
	 * and colors available!
	 * @return Query string.
	 */
	string queryString();

	/**
	 * Generates an answer string for a query object from the FFJSON tree. Query
	 * object is the FFJSON object returned by FFJSON(queryString).
	 * E.g.
	 * {animals:{horses:{count:35,colors:["white","black","brown"]}}}
	 * can be the answer string for 
	 * {animals:{horses:{count:?,colors:?}}}
	 * query string.
	 * @param queryStirng
	 * @return Answer string
	 */
	FFJSON* answerString(FFJSON& queryObject);

	FFJSON* answerObject(FFJSON* queryObject, FFJSONPObj* pObj = NULL,
			FerryTimeStamp lastUpdateTime = FerryTimeStamp());

	void erase(string name);

	void erase(int index);

	void erase(FFJSON* value);

	Iterator begin();

	Iterator end();

	Iterator find(string key);

	void headTheHeader(FFJSONPrettyPrintPObj& lfpo);
    
    void SelfTest();

	FFJSON& operator[](const char* prop);
	FFJSON& operator[](string prop);
	FFJSON& operator[](int index);

    /**
	 * returns null FFJSON object if invalid pointer. Deletes the object on
	 * deleting this FFJSON object if its the last reference.
	 * @param t pointer to any object
	 * @return FFJSON object of BINARY type
	 */
	template <typename T>
	FFJSON& operator=(T const& t) {
		freeObj();
        ffl_debug(FFJ_MAIN,"size:%d",sizeof(T));
        cout<<sizeof(T)<<endl;
        size = sizeof (T);
		val.vptr = new T(t);
		setType(BINARY);
        return *this;
	}
    FFJSON& operator=(const char* s);
	FFJSON& operator=(const string& s);
	FFJSON& operator=(const int& i);
	FFJSON& operator=(const unsigned int& i);
	FFJSON& operator=(const double& d);
	FFJSON& operator=(const float& f);
	FFJSON& operator=(const short& s);
	FFJSON& operator=(const long& l);
	FFJSON& operator=(const FFJSON& f);
	FFJSON& operator=(FFJSON* f);

	template<typename T>
	operator T&() {
        ffl_debug(FFJ_MAIN,"size:%d",sizeof(T));
        cout<<sizeof(T)<<endl;
        if (isType(BINARY) && size == sizeof(T)) {
			return *reinterpret_cast<T*> (val.vptr);
		}
		return *reinterpret_cast<T*> (NULL);
	}
	operator FFJSON&();
	operator const char*();
	operator double();
	operator float();
	operator bool();
	operator int();
	operator unsigned int();

private:
	uint32_t flags;
	FeaturedMember m_uFM;
	void copy(const FFJSON& orig, COPY_FLAGS cf = COPY_NONE,
			FFJSONPObj* pObj = NULL);
	static int getIndent(const char* ffjson, int* ci, int indent);
	static void strObjMapInit();
	static bool inline isWhiteSpace(char c);
	static bool inline isTerminatingChar(char c);
	static std::map<FFJSON*, set<FFJSONIterator> > sm_mUpdateObjs;
	FFJSON* returnNameIfDeclared(vector<string>& prop, FFJSONPObj* fpo) const;
	FFJSON* markTheNameIfExtended(FFJSONPrettyPrintPObj* fpo);
	bool inherit(FFJSON& obj, FFJSONPObj* pFPObj);
	void ReadMultiLinesInContainers(const string& ffjson, int& i,
			FFJSONPObj& pObj);
	string ConstructMultiLineStringArray(vector<FFJSON*>& vpfMulLnStrs,
			int indent, vector<int>& vClWidths) const;
	LinkNRef GetLinkString(FFJSONPObj* pObj);
};

ostream& operator<<(ostream& out, const FFJSON& f);

#endif
