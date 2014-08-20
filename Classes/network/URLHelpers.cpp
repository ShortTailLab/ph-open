//
//  URLHelpers.cpp
//  HelloCpp
//
//  Created by R.C on 12-10-30.
//
//

#include "URLHelpers.h"
#include "utf8.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/range.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <sha1.h>

using namespace std;
using namespace utf8;
using namespace PH::URLHelper;
using namespace boost::algorithm;
using namespace boost;
using namespace rapidjson;

wstring PH::URLHelper::str2wstr(const string& in)
{
    wstring r;
    utf8::utf8to16(in.begin(), in.end(), back_inserter(r));
    return r;
}

string PH::URLHelper::wstr2str(const wstring& in)
{
    string r;
    utf8::utf16to8(in.begin(), in.end(), back_inserter(r));
    return r;
}

std::string PH::URLHelper::getURLParameterValue(const std::string& url, const std::string& paramter, char querySep)
{
    size_t pos = url.find_first_of(querySep);
    
    if (pos != string::npos)
    {
        string urlParam;
        string encodedparam = urlEncode(paramter);
        
        copy(url.begin()+pos+1, url.end(), back_inserter(urlParam));
        trim_right_if(urlParam, is_any_of("#"));
        
        std::vector<string> splitResult;
        for (auto it=make_split_iterator(urlParam, first_finder("&"));it!=split_iterator<string::iterator>();++it)
        {
            splitResult.clear();
            split(splitResult, *it, is_any_of("="));
            if (splitResult.size() == 2 && splitResult[0] == encodedparam)
                return urlDecode(splitResult[1]);
        }
    }
    return "";
}

static inline
std::string encimpl(std::string::value_type v)
{
    std::string ret = std::string()+v;
    if (ret != "_" && !isalnum(v))
        ret = "%" + boost::algorithm::hex(ret);

    return ret;
}

std::string PH::URLHelper::urlEncode(const std::string& url)
{
    std::string qstr;
    
    for (auto c : url)
        qstr.append(encimpl(c));

    return qstr;
}

std::string PH::URLHelper::urlDecode(const std::string& value)
{
    std::string str;
    for (auto it=value.begin();it!=value.end();++it)
    {
        if (*it == '%' && value.end()-it >=3)
        {
            boost::algorithm::unhex(++it, it+2, back_inserter(str));
            it+=2;
        }
        else
            str += *it;
    }
    return str;
}

std::string PH::URLHelper::toDispositionFormString(const rapidjson::Value& params, const std::string& boundary)
{
    std::vector<string> paramVec;
    
    for (auto mb=params.MemberBegin();mb<params.MemberEnd();++mb)
    {
        string name(mb->name.GetString(), mb->name.GetStringLength());
        string value(mb->value.GetString(), mb->value.GetStringLength());
        paramVec.push_back("Content-Disposition: form-data; name=\"" + name + "\";\r\n\r\n" + value + "\r\n");
    }
    
    return boundary + boost::algorithm::join(paramVec, boundary);
}

std::string PH::URLHelper::toQueryString(const rapidjson::Value& p)
{
    std::vector<std::string> params;
    
    std::transform(p.MemberBegin(), p.MemberEnd(), back_inserter(params), [](const rapidjson::Value::Member& p)->string{
        string second;
        {
            if (p.value.IsString())
                second = string(p.value.GetString(), p.value.GetStringLength());
            else if (p.value.IsInt())
                second = boost::lexical_cast<string>(p.value.GetInt());
            else if (p.value.IsDouble())
                second = boost::lexical_cast<string>(p.value.GetDouble());
            else if (p.value.IsTrue())
                second = "true";
            else if (p.value.IsFalse())
                second = "false";
            else
                puts("Unsupported param type.");
        }
        return urlEncode(string(p.name.GetString(), p.name.GetStringLength())) + "=" + urlEncode(second);
    });
    
    return boost::algorithm::join(params, "&");
}

rapidjson::Document PH::URLHelper::copyValue(const rapidjson::Value& v)
{
    Document ret;
    copyValueTo(v, ret);

    return ret;
}

void PH::URLHelper::copyValueTo(const rapidjson::Value& v, rapidjson::Document& dest)
{
    dest.SetNull();
    
    StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
    v.Accept(writer);
    
    dest.Parse<0>(buffer.GetString());
}

std::string PH::URLHelper::write(const rapidjson::Value& v)
{
    if (v.IsNull())
        return "";
    
    StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
    v.Accept(writer);
    
    return string(buffer.GetString());
}

std::string PH::URLHelper::sha1FromJson(const rapidjson::Value& v)
{
    std::string str = write(v);
    std::string hash = sha1::hash(str.c_str(), str.size());
    return hash;
}
