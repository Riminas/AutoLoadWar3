#include "pch.h"
#include <ranges>
#include <string>

#include "StringToString.h"
#include "DataMaps.h"

DataMaps G_DATA_MAPS;

void DataMaps::NameMaps() {
    StringToString stringToString_;
    auto nameFileRemove = stringToString_.removeString(m_NameMapsFull);

    m_NameMaps = nameFileRemove[0];
    for (const std::wstring& part : nameFileRemove | std::views::drop(1)) {
        m_NameMaps += L'_' + part;
    }
}
