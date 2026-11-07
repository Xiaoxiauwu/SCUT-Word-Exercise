#include "UIInternal.h"

namespace scut_ui {

void SaveJsonToFile(Json::Value R, std::string path) {
    Json::StreamWriterBuilder writebuild;
    writebuild["emitUTF8"] = true;  //utf8支持,加这句,utf8的中文字符会编程\uxxx

    string document = Json::writeString(writebuild, R);
    //cout << R;
    //cout << "保存用户信息成功";
    writeFileFromString(path, document);
}

void SaveUserData() {
    Json::StreamWriterBuilder writebuild;
    writebuild["emitUTF8"] = true;  //utf8支持,加这句,utf8的中文字符会编程\uxxx

    string document = Json::writeString(writebuild, userInfo);
    //cout << R;
    //cout << "保存用户信息成功";
    writeFileFromString("userinfo.dat", ENCODE(document));

}

void EXIT() {
    SaveUserData();
    exit(0);
}

std::vector<std::wstring> GetAllFiles(std::wstring path, std::wstring fileType) {
    std::vector<std::wstring> files;
    WIN32_FIND_DATA findData{};
    HANDLE hFind = FindFirstFile((path + fileType).c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        //std::cout << "Failed to find first file!\n";DEBUG
        return files;
    }

    do {
        // 忽略"."和".."两个结果 
        if (wcscmp(findData.cFileName, L".") == 0 || wcscmp(findData.cFileName, L"..") == 0)   //strcmp 比较字符，相同则返回0
            continue;
        files.push_back(findData.cFileName);
    } while (FindNextFile(hFind, &findData));
    FindClose(hFind);
    return files;
}

void refresh() {//刷新新题集
    auto problems = GetAllFiles(L"problemSet", L"\\*.json");

    for (auto& pro : problems) {
        string rawInfo = readFileIntoString(encode::wideToOme(std::wstring(L"problemSet\\") + pro).c_str());
        //std::cout << rawInfo << "[";
        Json::Value R = readJsonFromString(rawInfo);

        //路径-名称 绑定
        problemSets.nameToPath[encode::UTF8_To_String(R["name"].asString())] = encode::wideToOme(std::wstring(L"problemSet\\") + pro);
        problemSets.pathToName[encode::wideToOme(std::wstring(L"problemSet\\") + pro)] = encode::UTF8_To_String(R["name"].asString());
        
        //std::cout << R["name"].asString() << "]\n";
        problemSets.sets[encode::UTF8_To_String(R["name"].asString())] = R;//重名会有覆盖，之后再改
    }
}

} // namespace scut_ui
