#pragma once
#include <wchar.h>
#include <cstdint>

#ifndef PARAM_STRUCTS
#define PARAM_STRUCTS
struct ParamTable {
    uint32_t ParamID;
    uint32_t Pad0x4;
    uint32_t ParamOffset;
    uint32_t Pad0xC;
    uint32_t StringOffset;
    uint32_t Pad0x14;
};
static_assert(sizeof(ParamTable) == 0x18);

struct ParamHeader {
    uint32_t StringOffset;
    uint8_t undefined0x4[0x6];
    uint16_t RowCount;
    uint8_t undefined0xC[0x4];
    uint64_t ParamTypeOffset;
    uint8_t undefined0x18[0x18];
    uint32_t DataOffset;
    uint8_t undefined0x34[0xC];
    ParamTable Table;
};
static_assert(sizeof(ParamHeader) == 0x58);

struct ParamInfo {
    uint8_t undefined0x0[0x18];
    wchar_t* ParamName;
    uint8_t undefined0x20[0x60];
    ParamHeader* Param;
};
static_assert(sizeof(ParamInfo) == 0x88);

struct ParamResCap {
    uint8_t undefined0x0[0x18];
    wchar_t* ParamName;
    uint8_t undefined0x20[0x8];
    uint32_t ParamNameLength;
    uint8_t undefined0x2C[0x54];
    ParamInfo* ParamInfo;
};
static_assert(sizeof(ParamResCap) == 0x88);
#endif // !PARAMS


template<typename T>
class ParamEditor {
public:
    explicit ParamEditor(uintptr_t soloParamRepository);

    T* AddEntry(uint32_t rowId) {
        //Don't add anything that already has an ID.
        if (find(_rowIDs.begin(), _rowIDs.end(), rowId) != _rowIDs.end())
            printf(
                "Trying to add a row that already exists in " + std::string(T::param_name) + "\n");

        _rowIDs.push_back(rowId);
        _rowData.template emplace_back(_defaultEntry);

        //I don't expect this to break, but it might?
        if (!_rowData.empty())
            return &_rowData.back();

        printf(
            "_rowData for " + std::string(T::param_name) + " is empty after trying to add a new entry.\n");
    }

    T* RetrieveEntry(uint32_t rowId) {
        //Return a pointer to the param row for editing. This will also allow editing of existing params.
        std::vector<uint32_t>::iterator findRef = find(_rowIDs.begin(), _rowIDs.end(), rowId);
        if (findRef == _rowIDs.end())
            printf("Trying to retrieve a row that doesn't exist in" + std::string(T::param_name) + "\n");

        int index = std::distance(_rowIDs.begin(), findRef);
        if (index < _paramHeader->RowCount)
            return (T*)(_paramPointer + _paramTable[index].ParamOffset);


        return &_rowData[index - _paramHeader->RowCount];
    }

    ParamHeader* GetParamHeader() {
        return _paramHeader;
    }

    void AddNewParams();

    T* GetParamDataPtr() {
        return (T*)(_paramPointer + _paramHeader->DataOffset);
    };

    std::vector<uint32_t> GetRowIds() {
        return _rowIDs;
    }

private:
    ParamResCap* getParamResCap();
    T _defaultEntry;
    std::vector<uint32_t> _rowIDs;
    std::vector<T> _rowData;
    ParamResCap* _paramResCap;
    ParamHeader* _paramHeader;
    ParamTable* _paramTable;
    uintptr_t _paramPointer;
    uintptr_t _soloParamRepository;
};

template<typename T>
ParamEditor<T>::ParamEditor(uintptr_t soloParamRepository) {
    _soloParamRepository = soloParamRepository;
    _paramResCap = getParamResCap();

    if (!_paramResCap) {
        printf("Param %s does not exist\n", std::string(T::param_name).c_str());
        return;
    }

    _paramHeader = _paramResCap->ParamInfo->Param;
    _paramPointer = (uintptr_t)_paramHeader;
    _paramTable = &_paramHeader->Table;

    std::string paramType = std::string((char*)(_paramPointer + _paramHeader->StringOffset));
    if (strcmp(paramType.c_str(), T::param_type) != 0) {
        printf("Param %s def strings name did not match. game: %s header: %s\n", std::string(T::param_name).c_str(), paramType.c_str(), std::string(T::param_type).c_str());
        return;
    }

    //Don't add params if the size of the param differs in game.
    size_t entrySize = _paramTable[1].ParamOffset - _paramTable[0].ParamOffset;
    if (entrySize != sizeof(T)){
        printf("Param %s new entries size and entry size in game did not match. In Game: %s - %s\n", std::string(T::param_name).c_str(), std::to_string(entrySize).c_str(), std::to_string(sizeof(T)).c_str());
        return;
    }

    for (int i = 0; i < _paramHeader->RowCount; ++i) {
        _rowIDs.push_back(_paramTable[i].ParamID);
    }

    _defaultEntry = T(*(T*)(_paramTable[0].ParamOffset + _paramPointer));
}

template<typename T>
ParamResCap* ParamEditor<T>::getParamResCap() {

    for (int i = 0; i < 186; i++) {
        int paramOffset = i * 0x48;
        int* param = (int*)(_soloParamRepository + paramOffset + 0x80);
        if (*param > 0) {
            ParamResCap* paramResCap = (ParamResCap*)*(uint64_t*)(_soloParamRepository + paramOffset + 0x88);
            wchar_t* containerName = (wchar_t*)&paramResCap->ParamName;

            if (paramResCap->ParamNameLength >= 8) {
                containerName = paramResCap->ParamName;
            }
            if (wcsncmp(T::param_namew, containerName, wcslen(T::param_namew)) == 0) {
                return paramResCap;
            }
        }
    }
    return nullptr;
}

