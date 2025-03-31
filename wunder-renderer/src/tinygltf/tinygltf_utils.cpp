
/*
 * Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-FileCopyrightText: Copyright (c) 2014-2024 NVIDIA CORPORATION
 * SPDX-License-Identifier: Apache-2.0
 */

#include "tinygltf/tinygltf_utils.h"

namespace tinygltf::utils {

KHR_materials_displacement get_displacement(const tinygltf::Material& tmat) {
  KHR_materials_displacement gmat;
  if (has_element_name(tmat.extensions,
                       KHR_MATERIALS_DISPLACEMENT_EXTENSION_NAME)) {
    const tinygltf::Value& ext = get_element_value(
        tmat.extensions, KHR_MATERIALS_DISPLACEMENT_EXTENSION_NAME);
    get_value(ext, "displacementGeometryTexture",
              gmat.m_displacement_geometry_texture);
    get_value(ext, "displacementGeometryFactor",
              gmat.m_displacement_geometry_factor);
    get_value(ext, "displacementGeometryOffset",
              gmat.m_displacement_geometry_offset);
  }
  return gmat;
}

KHR_materials_emissive_strength get_emissive_strength(
    const tinygltf::Material& tmat) {
  KHR_materials_emissive_strength gmat;
  if (has_element_name(tmat.extensions,
                       KHR_MATERIALS_EMISSIVE_STRENGTH_EXTENSION_NAME)) {
    const tinygltf::Value& ext = get_element_value(
        tmat.extensions, KHR_MATERIALS_EMISSIVE_STRENGTH_EXTENSION_NAME);
    get_value(ext, "emissiveStrength", gmat.emissiveStrength);
  }
  return gmat;
}

KHR_materials_volume get_volume(const tinygltf::Material& tmat) {
  KHR_materials_volume gmat;
  if (has_element_name(tmat.extensions, KHR_MATERIALS_VOLUME_EXTENSION_NAME)) {
    const tinygltf::Value& ext =
        get_element_value(tmat.extensions, KHR_MATERIALS_VOLUME_EXTENSION_NAME);
    get_value(ext, "thicknessFactor", gmat.m_thickness_factor);
    get_value(ext, "thicknessTexture", gmat.m_thickness_texture);
    get_value(ext, "attenuationDistance", gmat.m_attenuation_distance);
    get_array_value(ext, "attenuationColor", gmat.m_attenuation_color);
  }
  return gmat;
}

KHR_materials_unlit get_unlit(const tinygltf::Material& tmat) {
  KHR_materials_unlit gmat;
  if (has_element_name(tmat.extensions, KHR_MATERIALS_UNLIT_EXTENSION_NAME)) {
    gmat.active = 1;
  }
  return gmat;
}

std::optional<KHR_materials_specular> get_specular(const tinygltf::Material& tmat) {
  if (has_element_name(tmat.extensions,
                       KHR_MATERIALS_SPECULAR_EXTENSION_NAME)) {
  KHR_materials_specular gmat;

    const tinygltf::Value& ext = get_element_value(
        tmat.extensions, KHR_MATERIALS_SPECULAR_EXTENSION_NAME);
    get_value(ext, "specularFactor", gmat.m_specular_factor);
    get_value(ext, "specularTexture", gmat.m_specular_texture);
    get_array_value(ext, "specularColorFactor",
                    gmat.m_specular_color_factor);
    get_value(ext, "specularColorTexture", gmat.m_specular_color_texture);
return gmat;
                       }
  return std::nullopt;
}

KHR_texture_transform getTextureTransform(const tinygltf::TextureInfo& tinfo) {
  KHR_texture_transform gmat;
  if (has_element_name(tinfo.extensions,
                       KHR_TEXTURE_TRANSFORM_EXTENSION_NAME)) {
    const tinygltf::Value& ext = get_element_value(
        tinfo.extensions, KHR_TEXTURE_TRANSFORM_EXTENSION_NAME);
    get_array_value(ext, "offset", gmat.offset);
    get_array_value(ext, "scale", gmat.scale);
    get_value(ext, "rotation", gmat.rotation);
    get_value(ext, "texCoord", gmat.texCoord);

    gmat.updateTransform();
  }
  return gmat;
}

KHR_materials_clearcoat get_clearcoat(const tinygltf::Material& tmat) {
  KHR_materials_clearcoat gmat;
  if (has_element_name(tmat.extensions,
                       KHR_MATERIALS_CLEARCOAT_EXTENSION_NAME)) {
    const tinygltf::Value& ext = get_element_value(
        tmat.extensions, KHR_MATERIALS_CLEARCOAT_EXTENSION_NAME);
    get_value(ext, "clearcoatFactor", gmat.m_factor);
    get_value(ext, "clearcoatTexture", gmat.m_texture);
    get_value(ext, "clearcoatRoughnessFactor", gmat.m_roughness_factor);
    get_value(ext, "clearcoatRoughnessTexture", gmat.m_roughness_texture);
    get_value(ext, "clearcoatNormalTexture", gmat.m_normal_texture);
  }
  return gmat;
}

KHR_materials_sheen get_sheen(const tinygltf::Material& tmat) {
  KHR_materials_sheen gmat;
  if (has_element_name(tmat.extensions, KHR_MATERIALS_SHEEN_EXTENSION_NAME)) {
    const tinygltf::Value& ext =
        get_element_value(tmat.extensions, KHR_MATERIALS_SHEEN_EXTENSION_NAME);
    get_array_value(ext, "sheenColorFactor", gmat.m_sheen_color_factor);
    get_value(ext, "sheenColorTexture", gmat.m_sheen_color_texture);
    get_value(ext, "sheenRoughnessFactor", gmat.m_sheen_roughness_factor);
    get_value(ext, "sheenRoughnessTexture", gmat.m_sheen_roughness_texture);
  }
  return gmat;
}

KHR_materials_transmission get_transmission(const tinygltf::Material& tmat) {
  KHR_materials_transmission gmat;
  if (has_element_name(tmat.extensions,
                       KHR_MATERIALS_TRANSMISSION_EXTENSION_NAME)) {
    const tinygltf::Value& ext = get_element_value(
        tmat.extensions, KHR_MATERIALS_TRANSMISSION_EXTENSION_NAME);
    get_value(ext, "transmissionFactor", gmat.factor);
    get_value(ext, "transmissionTexture", gmat.texture);
  }
  return gmat;
}

KHR_materials_anisotropy get_anisotropy(const tinygltf::Material& tmat) {
  KHR_materials_anisotropy gmat;
  if (has_element_name(tmat.extensions,
                       KHR_MATERIALS_ANISOTROPY_EXTENSION_NAME)) {
    const tinygltf::Value& ext = get_element_value(
        tmat.extensions, KHR_MATERIALS_ANISOTROPY_EXTENSION_NAME);
    get_value(ext, "anisotropyStrength", gmat.m_anisotropy_strength);
    get_value(ext, "anisotropyRotation", gmat.m_anisotropy_rotation);
    get_value(ext, "anisotropyTexture", gmat.m_anisotropy_texture);
  }
  return gmat;
}

KHR_materials_ior get_ior(const tinygltf::Material& tmat) {
  KHR_materials_ior gmat;
  if (has_element_name(tmat.extensions, KHR_MATERIALS_IOR_EXTENSION_NAME)) {
    const tinygltf::Value& ext =
        get_element_value(tmat.extensions, KHR_MATERIALS_IOR_EXTENSION_NAME);
    get_value(ext, "ior", gmat.ior);
  }
  return gmat;
}

KHR_materials_iridescence get_iridescence(const tinygltf::Material& tmat) {
  KHR_materials_iridescence gmat;
  if (has_element_name(tmat.extensions,
                       KHR_MATERIALS_IRIDESCENCE_EXTENSION_NAME)) {
    const tinygltf::Value& ext = get_element_value(
        tmat.extensions, KHR_MATERIALS_IRIDESCENCE_EXTENSION_NAME);

    get_value(ext, "iridescenceFactor", gmat.m_iridescence_factor);
    get_value(ext, "iridescenceTexture", gmat.m_iridescence_texture);
    get_value(ext, "iridescenceIor", gmat.m_iridescence_ior);
    get_value(ext, "iridescenceThicknessMinimum",
              gmat.m_iridescence_thickness_minimum);
    get_value(ext, "iridescenceThicknessMaximum",
              gmat.m_iridescence_thickness_maximum);
    get_value(ext, "iridescenceThicknessTexture",
              gmat.m_iridescence_thickness_texture);
  }
  return gmat;
}

tinygltf::Value convert_to_tinygltf_value(int numElements,
                                          const float* elements) {
  tinygltf::Value::Array result;
  result.reserve(numElements);

  for (int i = 0; i < numElements; ++i) {
    result.emplace_back(static_cast<double>(elements[i]));
  }

  return tinygltf::Value(result);
}

void get_node_trs(const tinygltf::Node& node, glm::vec3& translation,
                  glm::quat& rotation, glm::vec3& scale) {
  // Initialize translation, rotation, and scale to default values
  translation = glm::vec3(0.0f, 0.0f, 0.0f);
  rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  scale = glm::vec3(1.0f, 1.0f, 1.0f);

  // Check if the node has a matrix defined
  if (node.matrix.size() == 16) {
    glm::mat4 matrix = glm::make_mat4(node.matrix.data());
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(matrix, scale, rotation, translation, skew, perspective);
    return;
  }

  // Retrieve translation if available
  if (node.translation.size() == 3) {
    translation = glm::make_vec3(node.translation.data());
  }

  // Retrieve rotation if available
  if (node.rotation.size() == 4) {
    rotation.x = float(node.rotation[0]);
    rotation.y = float(node.rotation[1]);
    rotation.z = float(node.rotation[2]);
    rotation.w = float(node.rotation[3]);
  }

  // Retrieve scale if available
  if (node.scale.size() == 3) {
    scale = glm::make_vec3(node.scale.data());
  }
}

void set_node_trs(tinygltf::Node& node, const glm::vec3& translation,
                  const glm::quat& rotation, const glm::vec3& scale) {
  node.translation = {translation.x, translation.y, translation.z};
  node.rotation = {rotation.x, rotation.y, rotation.z, rotation.w};
  node.scale = {scale.x, scale.y, scale.z};
}

glm::mat4 get_node_matrix(const tinygltf::Node& node) {
  if (node.matrix.size() == 16) {
    return glm::make_mat4(node.matrix.data());
  }

  glm::vec3 translation;
  glm::quat rotation;
  glm::vec3 scale;
  get_node_trs(node, translation, rotation, scale);

  return glm::translate(glm::mat4(1.0f), translation) *
         glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f), scale);
}

std::string generate_primitive_key(const tinygltf::Primitive& primitive) {
  std::stringstream o;
  for (const auto& kv : primitive.attributes) {
    o << kv.first << ":" << kv.second << " ";
  }
  return o.str();
}

glm::mat4 getLocalMatrix(const tinygltf::Node& tnode) {
  glm::mat4 mtranslation{1};
  glm::mat4 mscale{1};
  glm::mat4 mrot{1};
  glm::mat4 matrix{1};
  glm::quat mrotation;

  if (!tnode.translation.empty())
    mtranslation = glm::translate(
        glm::mat4(1), glm::vec3(tnode.translation[0], tnode.translation[1],
                                tnode.translation[2]));
  if (!tnode.scale.empty())
    mscale = glm::scale(glm::mat4(1), glm::vec3(tnode.scale[0], tnode.scale[1],
                                                tnode.scale[2]));
  if (!tnode.rotation.empty()) {
    mrotation = glm::make_quat(tnode.rotation.data());
    mrot = glm::mat4_cast(mrotation);
  }
  if (!tnode.matrix.empty()) {
    matrix = glm::make_mat4(tnode.matrix.data());
  }
  return mtranslation * mrot * mscale * matrix;
}
}  // namespace tinygltf::utils
