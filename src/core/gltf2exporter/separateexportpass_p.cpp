/*
    separateexportpass_cpp.h

    This file is part of Kuesa.

    Copyright (C) 2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jean-Michaël Celerier <jean-michael.celerier@kdab.com>

    Licensees holding valid proprietary KDAB Kuesa licenses may use this file in
    accordance with the Kuesa Enterprise License Agreement provided with the Software in the
    LICENSE.KUESA.ENTERPRISE file.

    Contact info@kdab.com if any conditions of this licensing are not clear to you.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "separateexportpass_p.h"
#include "gltf2keys_p.h"
#include "gltf2exporter_p.h"
#include "gltf2uri_p.h"
#include <QJsonArray>

QT_BEGIN_NAMESPACE
namespace Kuesa {

/*!
 * \class SeparateExportPass
 * \brief glTF export pass that takes all the base64-encoded embedded buffers and saves them in files.
 */
SeparateExportPass::SeparateExportPass(const QDir &destination)
    : m_destination(destination)
{
}

const QStringList &SeparateExportPass::errors() const
{
    return m_errors;
}

const QStringList &SeparateExportPass::generatedFiles() const
{
    return m_generated;
}

void SeparateExportPass::separateURIsInArray(QJsonObject &root, const QLatin1String &key)
{
    auto array_it = root.find(key);
    if (array_it == root.end())
        return;

    auto array = array_it->toArray();
    for (QJsonValueRef val_v : array) {
        auto obj = val_v.toObject();
        if (separateEncodedUri(obj)) {
            val_v = obj;
        }
    }
    *array_it = array;
}

bool SeparateExportPass::separateEncodedUri(QJsonObject &object)
{
    using namespace GLTF2Import;
    const auto uri_it = object.find(GLTF2Import::KEY_URI);
    if (uri_it == object.end())
        return false;

    const auto &uri = uri_it->toString();
    if (Uri::kind(uri) != Uri::Kind::Data)
        return false;

    const auto data = Uri::parseEmbeddedData(uri);
    const auto file = generateUniqueFilename(m_destination, QStringLiteral("extractedBuffer-%1.bin").arg(m_extractedCount));
    const auto filepath = m_destination.filePath(file);
    QFile resource(filepath);
    if (!resource.open(QIODevice::WriteOnly)) {
        m_errors << QStringLiteral("Cannot create: %1").arg(uri);
        return false;
    }
    resource.write(data);

    *uri_it = resource.fileName();

    m_generated.push_back(filepath);
    ++m_extractedCount;
    return true;
}

} // namespace Kuesa
QT_END_NAMESPACE
