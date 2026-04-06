#include <rest.h>
RestResponse::RestResponse() {
	Body = *CreateString("");
}
RestResponse::~RestResponse() {

}
JValue* RestResponse::GetJsonBody() const {
    if (Body.empty()) return nullptr;
    JValue* jv = new JValue();
    try {
        auto jv = new JValue();
        jv->json = nlohmann::json::parse(Body.c_str(), nullptr, true, true);
        return jv;

    }
    catch (const nlohmann::json::parse_error& e)
    {
    }
    return nullptr;
}