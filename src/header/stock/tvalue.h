#pragma  once
#include <filesystem>
#define TVALUE_TYPE_UNDEFINED  0
#define TVALUE_TYPE_OBJECT     1
#define TVALUE_TYPE_ARRAY      2
#define TVALUE_TYPE_STRING     3
#define TVALUE_TYPE_NUMBER     4
#define TVALUE_TYPE_BOOLEAN    5


inline toml::node* CloneNode(const toml::node& n)
{
    try
    {
        if (n.is_table())
        {
            auto* tbl = n.as_table();
            if (!tbl)
                return nullptr;

            return new toml::table(*tbl);
        }
        if (n.is_array())
        {
            auto* arr = n.as_array();
            if (!arr)
                return nullptr;

            return new toml::array(*arr);
        }
        if (n.is_string())
            return new toml::value<std::string>(n.as_string()->get());

        if (n.is_integer())
            return new toml::value<int64_t>(n.as_integer()->get());

        if (n.is_floating_point())
            return new toml::value<double>(n.as_floating_point()->get());

        if (n.is_boolean())
            return new toml::value<bool>(n.as_boolean()->get());
        return nullptr;
    }
    catch (...)
    {
    }

    return nullptr;
}
template<typename T>
bool TomlTryConvert(const toml::node& n, T& out)
{
    try
    {
        if constexpr (std::is_same_v<T, std::string>)
        {
            if (auto v = n.as_string())
            {
                out = v->get();
                return true;
            }
            if (auto i = n.as_integer())
            {
                out = std::to_string(i->get());
                return true;
            }
            if (auto f = n.as_floating_point())
            {
                out = std::to_string(f->get());
                return true;
            }
            if (auto b = n.as_boolean())
            {
                out = b->get() ? "true" : "false";
                return true;
            }
        }
        else if constexpr (std::is_same_v<T, asINT32>)
        {
            if (auto i = n.as_integer())
            {
                out = (asINT32)i->get();
                return true;
            }
            if (auto s = n.as_string())
            {
                out = std::stoi(s->get());
                return true;
            }
        }
        else if constexpr (std::is_same_v<T, float>)
        {
            if (auto f = n.as_floating_point())
            {
                out = (float)f->get();
                return true;
            }
            if (auto i = n.as_integer())
            {
                out = (float)i->get();
                return true;
            }
        }
        else if constexpr (std::is_same_v<T, double>)
        {
            if (auto f = n.as_floating_point())
            {
                out = f->get();
                return true;
            }
            if (auto i = n.as_integer())
            {
                out = (double)i->get();
                return true;
            }
        }
        else if constexpr (std::is_same_v<T, bool>)
        {
            if (auto b = n.as_boolean())
            {
                out = b->get();
                return true;
            }
            if (auto i = n.as_integer())
            {
                out = i->get() != 0;
                return true;
            }
            if (auto s = n.as_string())
            {
                std::string v = s->get();
                out = (v == "true" || v == "1");
                return true;
            }
        }
    }
    catch (...)
    {
    }

    return false;
};

class TValue : public CASBaseGCObject
{
private:
    toml::node* node;

public:
    TValue(toml::node* _node)
        
    {
        node = _node;
    }
    template<typename T>
    T GetValue(const char* key, bool isPath, T defaultValue)
    {
        toml::node* n = ResolveNode(key, isPath);
        if (!n)
            return defaultValue;

        T result{};
        if (TomlTryConvert<T>(*n, result))
            return result;

        return defaultValue;
    };
    template<typename T>
    bool SetValue(const char* key, const T& value)
    {
        if (!node)
            return false;
        if (!node->is_table())
            return false;
        auto* tbl = node->as_table();
        if (!tbl)
            return false;
        try
        {
            tbl->insert_or_assign(key, value);
            return true;
        }
        catch (...)
        {
        }
        return false;
    }
    TValue* Get(CString& key, bool isPath)
    {
		auto* it = ResolveNode(key.c_str(), isPath);
		if (!it)
			return nullptr;
        return new TValue(CloneNode(*it));
    }
    bool Set(CString& key, TValue* value, bool isPath)
    {
        if (!node || !value || !value->node)
            return false;
        try
        {
            if (!isPath)
            {
                if (!node->is_table())
                    return false;
                auto* tbl = node->as_table();
                if (!tbl)
                    return false;

                tbl->insert_or_assign(
                    key.c_str(),
                    *value->node
                );

                return true;
            }
        }
        catch (...)
        {
        }
        return false;
    }
    toml::node* ResolveNode(const char* key, bool isPath)
    {
        if (!node)
            return nullptr;

        if (!key || !*key)
            return node;

        try
        {
            if (!isPath)
            {
                if (!node->is_table())
                    return nullptr;

                auto* tbl = node->as_table();
                if (!tbl)
                    return nullptr;

                auto it = tbl->find(key);

                if (it == tbl->end())
                    return nullptr;

                return &it->second;
            }

            toml::node* current = node;

            std::string path(key);
            size_t i = 0;

            while (i < path.size())
            {
                std::string part;

                while (i < path.size() &&
                    path[i] != '.' &&
                    path[i] != '[')
                {
                    part += path[i];
                    ++i;
                }
                if (!part.empty())
                {
                    if (!current->is_table())
                        return nullptr;

                    auto* tbl = current->as_table();

                    if (!tbl)
                        return nullptr;

                    auto it = tbl->find(part);

                    if (it == tbl->end())
                        return nullptr;

                    current = &it->second;
                }

                while (i < path.size() && path[i] == '[')
                {
                    ++i;

                    std::string num;

                    while (i < path.size() && path[i] != ']')
                    {
                        num += path[i];
                        ++i;
                    }

                    if (i >= path.size())
                        return nullptr;
                    ++i;

                    if (!current->is_array())
                        return nullptr;

                    auto* arr = current->as_array();

                    if (!arr)
                        return nullptr;

                    char* end = nullptr;

                    long index =
                        std::strtol(num.c_str(), &end, 10);

                    if (*end != '\0')
                        return nullptr;

                    if (index < 0 ||
                        (size_t)index >= arr->size())
                        return nullptr;

                    current = &(*arr)[(size_t)index];
                }

                // skip .
                if (i < path.size() && path[i] == '.')
                    ++i;
            }

            return current;
        }
        catch (...)
        {
        }

        return nullptr;
    }
    CString* GetString(CString& key, bool isPath)
    {
        std::string out = GetValue<std::string>(key.c_str(), isPath, "");
		CString* result = new CString();
		result->assign(out.c_str(), out.size());
        return result;
    }
    bool GetBool(CString& key, bool isPath)
    {
        return GetValue<bool>(key.c_str(), isPath, false);
    }
    asINT32 GetInt(CString& key, bool isPath)
    {
        return GetValue<asINT32>(key.c_str(), isPath, 0);
    }
    asINT64 GetInt64(CString& key, bool isPath)
    {
        return GetValue<asINT64>(key.c_str(), isPath, 0);
    }
    float GetFloat(CString& key, bool isPath)
    {
        return GetValue<float>(key.c_str(), isPath, 0);
    }
    double GetDouble(CString& key, bool isPath)
    {
        return GetValue<double>(key.c_str(), isPath, 0);
    }
	bool IsArray(CString& key, bool isPath)
	{
		auto* n = ResolveNode(key.c_str(), isPath);
		return n && n->is_array();
	}
    bool SetString(CString& key, CString& value)
    {
        return SetValue(key.c_str(), std::string(value.c_str(), value.size()));
    }
    bool SetInt(CString& key, asINT32 value)
    {
        return SetValue(key.c_str(), value);
    }
    bool SetBool(CString& key, bool value)
    {
        return SetValue(key.c_str(), value);
    }
    bool SetFloat(CString& key, float value)
    {
        return SetValue(key.c_str(), value);
    }
    bool SetDouble(CString& key, double value)
    {
        return SetValue(key.c_str(), value);
    }
	bool SetInt64(CString& key, asINT64 value)
	{
		return SetValue(key.c_str(), value);
	}
    CString* ToString();
    void SaveToFile(CString& path, int indent);
    asUINT GetSize()
    {
        auto* arr = node->as_array();
        if (!arr)
            return 0;
        return (asUINT)arr->size();
    }
    TValue* At(asUINT index)
    {
        auto* arr = node->as_array();
        if (!arr)
            return nullptr;
        if (index >= arr->size())
            return nullptr;
        toml::node& n = (*arr)[index];
        return new TValue(CloneNode(n));
    }
    unsigned int GetType()
    {
        if (!node)
            return TVALUE_TYPE_UNDEFINED;
        if (node->is_table())
            return TVALUE_TYPE_OBJECT;
        if (node->is_array())
            return TVALUE_TYPE_ARRAY;
        if (node->is_string())
            return TVALUE_TYPE_STRING;
        if (node->is_integer() ||
            node->is_floating_point())
            return TVALUE_TYPE_NUMBER;
        if (node->is_boolean())
            return TVALUE_TYPE_BOOLEAN;
        return TVALUE_TYPE_UNDEFINED;
    }
    bool Equals(const TValue& other)
    {
        if (!node || !other.node)
            return false;
        try
        {
            if (node->type() != other.node->type())
                return false;

            if (node->is_string())
                return node->as_string()->get() ==
                other.node->as_string()->get();

            if (node->is_integer())
                return node->as_integer()->get() ==
                other.node->as_integer()->get();

            if (node->is_floating_point())
                return node->as_floating_point()->get() ==
                other.node->as_floating_point()->get();

            if (node->is_boolean())
                return node->as_boolean()->get() ==
                other.node->as_boolean()->get();

            if (node->is_table() || node->is_array())
            {
                std::stringstream a, b;
                a << toml::default_formatter{ *node };
                b << toml::default_formatter{ *other.node };

                return a.str() == b.str();
            }
        }
        catch (...)
        {
        }

        return false;
    }
    void Clear()
    {
		if (!node)
			return;
		try
		{
            if(node->is_table())
			if (auto* tbl = node->as_table())
			{
				tbl->clear();
			}
            if(node->is_array())
			 if (auto* arr = node->as_array())
			{
				arr->clear();
			}
		}
		catch (...)
		{
		}
    }
};
void RegisterTValue(asIScriptEngine* engine);