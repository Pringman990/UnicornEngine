#pragma once
#include <yaml-cpp/yaml.h>
#include "StringConvert.h"

namespace YAML {
    template<>
    struct convert<Vector3> {
        static Node encode(const Vector3& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, Vector3& rhs) {
            if (!node.IsSequence() || node.size() != 3) {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<Vector2> {
        static Node encode(const Vector2& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }

        static bool decode(const Node& node, Vector2& rhs) {
            if (!node.IsSequence() || node.size() != 2) {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template<>
    struct convert<Vector4> {
        static Node encode(const Vector4& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }

        static bool decode(const Node& node, Vector4& rhs) {
            if (!node.IsSequence() || node.size() != 4) {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    template<>
    struct convert<Matrix> {
        static Node encode(const Matrix& rhs) {
            Node node;

            node.push_back(rhs.m[0][0]);
            node.push_back(rhs.m[0][1]);
            node.push_back(rhs.m[0][2]);
            node.push_back(rhs.m[0][3]);

            node.push_back(rhs.m[1][0]);
            node.push_back(rhs.m[1][1]);
            node.push_back(rhs.m[1][2]);
            node.push_back(rhs.m[1][3]);

            node.push_back(rhs.m[2][0]);
            node.push_back(rhs.m[2][1]);
            node.push_back(rhs.m[2][2]);
            node.push_back(rhs.m[2][3]);

            node.push_back(rhs.m[3][0]);
            node.push_back(rhs.m[3][1]);
            node.push_back(rhs.m[3][2]);
            node.push_back(rhs.m[3][3]);
            return node;
        }

        static bool decode(const Node& node, Matrix& rhs) {
            if (!node.IsSequence() || node.size() != 16) {
                return false;
            }

            Matrix matrix(
                node[0].as<float>(), node[1].as<float>(), node[2].as<float>(), node[3].as<float>(),
                node[4].as<float>(), node[5].as<float>(), node[6].as<float>(), node[7].as<float>(),
                node[8].as<float>(), node[9].as<float>(), node[10].as<float>(), node[11].as<float>(),
                node[12].as<float>(), node[13].as<float>(), node[14].as<float>(), node[15].as<float>()
            );

            rhs = matrix;
            return true;
        }
    };

    template<>
    struct convert<Color> {
        static Node encode(const Color& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }

        static bool decode(const Node& node, Color& rhs) {
            if (!node.IsSequence() || node.size() != 4) {
                return false;
            }
            
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    template<>
    struct convert<std::wstring> {
        static Node encode(const std::wstring& rhs) {
            return Node(WStringToString(rhs));
        }

        static bool decode(const Node& node, std::wstring& rhs) {
            if (!node.IsScalar()) {
                return false;
            }
            rhs = StringToWString(node.as<std::string>());
            return true;
        }
    };

    inline YAML::Emitter& operator << (YAML::Emitter& out, const Vector2& v) {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
        return out;
    }

    inline YAML::Emitter& operator << (YAML::Emitter& out, const Vector3& v) {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }

    inline YAML::Emitter& operator << (YAML::Emitter& out, const Vector4& v) {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    inline YAML::Emitter& operator << (YAML::Emitter& out, const Matrix& v) {
        out << YAML::Flow;
        out << YAML::BeginSeq 
            << v.m[0][0] << v.m[0][1] << v.m[0][2] << v.m[0][3]
            << v.m[1][0] << v.m[1][1] << v.m[1][2] << v.m[1][3]
            << v.m[2][0] << v.m[2][1] << v.m[2][2] << v.m[2][3]
            << v.m[3][0] << v.m[3][1] << v.m[3][2] << v.m[3][3]
            << YAML::EndSeq;
        return out;
    }

    inline YAML::Emitter& operator << (YAML::Emitter& out, const Color& v) {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    inline YAML::Emitter& operator << (YAML::Emitter& out, const wchar_t*& v) {
        out << WStringToString(v);
        return out;
    }

    inline YAML::Emitter& operator << (YAML::Emitter& out, const std::wstring& v) {
        out << WStringToString(v);
        return out;
    }
}