//
//  URLHelpers.h
//  HelloCpp
//
//  Created by R.C on 12-10-30.
//
//

#ifndef __HelloCpp__URLHelpers__
#define __HelloCpp__URLHelpers__

#include <string>
#include <rapidjson/document.h>

namespace PH
{
    namespace URLHelper
    {
        std::string urlEncode(const std::string& value);
        std::string urlDecode(const std::string& value);
        
        std::string toDispositionFormString(const rapidjson::Value& p, const std::string& boundary);
        std::string toQueryString(const rapidjson::Value&);
        
        std::string getURLParameterValue(const std::string& url, const std::string& paramter, char querySep='?');
        
        std::wstring str2wstr(const std::string&);
        std::string wstr2str(const std::wstring&);
        
        rapidjson::Document copyValue(const rapidjson::Value& v);
        void copyValueTo(const rapidjson::Value& v, rapidjson::Document&);
    
        std::string write(const rapidjson::Value& v);
        
        std::string sha1FromJson(const rapidjson::Value& v);
    }
}

#endif /* defined(__HelloCpp__URLHelpers__) */
