/*
    dracoexportpass_p.h

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

#ifndef KUESA_GLTF2EXPORTER_DRACOEXPORTPASS_P_H
#define KUESA_GLTF2EXPORTER_DRACOEXPORTPASS_P_H

//
//  NOTICE
//  ------
//
// We mean it: this file is not part of the public API and could be
// modified without notice
//

#include <qtkuesa-config.h>
#if defined(KUESA_DRACO_COMPRESSION)
#include <draco/compression/encode.h>

#include "embedexportpass_p.h"
#include "dracocompressor_p.h"
#include "gltf2exporter_p.h"
#include "gltf2keys_p.h"
#include "gltf2uri_p.h"
#include "gltf2importer.h"
#include "gltf2context_p.h"
#include <SceneEntity>
#include "kuesa_p.h"

#include <set>

QT_BEGIN_NAMESPACE
namespace Kuesa {
class DracoExportPass
{
public:
    DracoExportPass(
            QDir source,
            QDir destination,
            const QJsonObject &rootObject,
            const GLTF2ExportConfiguration &conf,
            GLTF2Import::GLTF2Context &context);

    const QString &compressedBufferFilename() const;

    QJsonObject compress();

private:
    struct CompressedGLTFPrimitive {
        QJsonObject primitiveJson;
        QByteArray compressedData;
        QJsonObject newBufferView;
        std::set<int> accessorsToClean;
    };

    QStringList m_errors;

    QJsonObject m_root;
    QJsonArray m_buffers;
    QJsonArray m_bufferViews;
    QJsonArray m_accessors;
    QJsonArray m_meshes;
    QJsonArray m_images;

    QByteArray m_compressedBuffer;
    const int m_compressedBufferIndex;
    int m_newBufferViewIndex;
    std::set<int> m_accessorsToClean;
    QDir m_basePath, m_destination;
    QString m_compressedBufferFilename;

    const GLTF2ExportConfiguration &m_conf;
    GLTF2Import::GLTF2Context &m_context;

    // Compress a single mesh
    QJsonObject compressMesh(QJsonObject mesh_json, int mesh_idx);

    // Compress a single primitive of a mesh
    CompressedGLTFPrimitive compressPrimitive(
            QJsonObject primitive_json, const Kuesa::GLTF2Import::Primitive &primitive) const;

    void removeBufferViews(const std::set<int> &indicesToRemove);

    struct BufferParts {
        int offset{};
        int length{};
        int stride{};
        friend bool operator==(const BufferParts &lhs, const BufferParts &rhs)
        {
            return lhs.offset == rhs.offset;
        }
        friend bool operator<(const BufferParts &lhs, const BufferParts &rhs)
        {
            return lhs.offset < rhs.offset;
        }
    };

    // Maps the new index of an offset in a buffer after removal of prior parts
    static int mapBufferOffset(const std::set<BufferParts> &removed_parts, int offset);

    // Removes the unused data in buffers
    void cleanupBuffers(const QJsonArray &removedBufferViews);

    // Given an accessor array, remove their "bufferView": key, and return the corresponding set of
    // bufferViews
    static std::set<int> cleanAccessors(QJsonArray &accessors, const std::set<int> &indices);
};

} // namespace Kuesa
QT_END_NAMESPACE

#endif
#endif // KUESA_GLTF2EXPORTER_DRACOEXPORTPASS_P_H
