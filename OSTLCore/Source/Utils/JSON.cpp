#include "Utils/JSON.h"
#include "API/rapidjson/StringBuffer.h"
#include "API/rapidjson/Writer.h"
using namespace oi;

JSON::JSON(OString fromString) {
	json.Parse(fromString.c_str());
}

JSON::JSON() : JSON("{}") {}

JSON::operator OString() {

	rapidjson::StringBuffer buffer;

	buffer.Clear();

	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	json.Accept(writer);

	return buffer.GetString();
}

u32 JSON::getMembers(OString path) {

	if (path == "")
		return json.MemberCount();

	rapidjson::Value *val;
	if (!getValue(path, val) || !(val->IsArray() || val->IsObject())) return 0;

	if (val->IsArray()) return (u32)val->GetArray().Size();

	return (u32)val->MemberCount();
}

bool JSON::getValue(OString path, rapidjson::Value *&oval) {

	if (path == "" || !json.IsObject()) return false;

	u32 i = 0;
	rapidjson::Value *val = nullptr;
	auto arr = path.split("/");

	for (OString sub : arr) {

		if (i == 0)
			if (!json.HasMember(sub.c_str())) return false;
			else 
				val = &json.FindMember(sub.c_str())->value;
		else {

			if (val->IsArray()) {
				if (!sub.isUint() || sub.toLong() >= val->GetArray().Size()) return false;
				val = &val->GetArray()[(rapidjson::SizeType)sub.toLong()];
				continue;
			}

			if (!val->IsObject()) {

				if (i == arr.size() - 1) {
					oval = val;
					return true;
				}
				else
					return false;

			}

			if (!val->HasMember(sub.c_str())) return false;
			val = &(*val)[sub.c_str()];
		}

		++i;
	}

	oval = val;
	return true;
}

std::vector<OString> JSON::getMemberNames(OString path) {

	std::vector<OString> res;

	if (path == "")
		for (u32 i = 0; i < json.MemberCount(); ++i)
			res.push_back((json.MemberBegin() + i)->name.GetString());

	rapidjson::Value *val;
	if (!getValue(path, val)) return res;

	if (val->IsArray()) {
		for (u32 i = 0; i < val->GetArray().Size(); ++i)
			res.push_back(path + "/" + i);
	}

	if (!val->IsObject()) return res;

	for (u32 i = 0; i < val->MemberCount(); ++i)
		res.push_back(path + "/" + (val->MemberBegin() + i)->name.GetString());

	return res;
}

std::vector<OString> JSON::getMemberIds(OString path) {

	std::vector<OString> res;

	if (path == "")
		for (u32 i = 0; i < json.MemberCount(); ++i)
			res.push_back((json.MemberBegin() + i)->name.GetString());

	rapidjson::Value *val;
	if (!getValue(path, val)) return res;

	if (val->IsArray()) {
		for (u32 i = 0; i < val->GetArray().Size(); ++i)
			res.push_back(i);
	}

	if (!val->IsObject()) return res;

	for (u32 i = 0; i < val->MemberCount(); ++i)
		res.push_back((val->MemberBegin() + i)->name.GetString());

	return res;
}

void JSON::getAllMembers(OString path, std::vector<OString> &out) {

	if (path == "")
		for (u32 i = 0; i < json.MemberCount(); ++i) {
			OString tpath = (json.MemberBegin() + i)->name.GetString();
			out.push_back(tpath);
			getAllMembers(tpath, out);
		}

	rapidjson::Value *val;
	if (!getValue(path, val)) return;

	if (val->IsArray()) {
		for (u32 i = 0; i < val->GetArray().Size(); ++i) {
			OString tpath = path + "/" + i;
			out.push_back(tpath);
			getAllMembers(tpath, out);
		}
	}

	if (!val->IsObject())
		return;

	for (u32 i = 0; i < val->MemberCount(); ++i) {
		OString tpath = path + "/" + (val->MemberBegin() + i)->name.GetString();
		out.push_back(tpath);
		getAllMembers(tpath, out);
	}
}

bool JSON::exists(OString path) {

	if (path == "") return true;

	rapidjson::Value *oval;
	return getValue(path, oval);
}

bool JSON::mkdir(OString path, bool useLists) {

	if (exists(path)) return true;

	u32 i = 0;
	OString cpath;

	rapidjson::Value *val;

	auto addElem = [&](rapidjson::Value *tval, OString name, rapidjson::Type type = rapidjson::Type::kObjectType) { tval->AddMember(rapidjson::Value(name.c_str(), name.size(), json.GetAllocator()), rapidjson::Value(type), json.GetAllocator()); };

	auto addElemToArr = [&](rapidjson::GenericArray<false, rapidjson::Value> &arr, rapidjson::Type type = rapidjson::Type::kObjectType) {
		arr.PushBack(rapidjson::Value(type), json.GetAllocator());
	};

	for (OString sub : path.split("/")) {

		OString tpath = cpath;
		cpath = (i == 0 ? "" : cpath + "/") + sub;

		bool isListIndex = sub.isUint();

		if (!getValue(cpath, val)) {

			if (i > 0) {

				rapidjson::Value *tval;
				getValue(tpath, tval);

				if (useLists && tval->IsObject() && tval->MemberCount() == 0 && sub.isUint()) {
					rapidjson::Value &tvalr = *tval;
					tvalr = rapidjson::Value(rapidjson::Type::kArrayType);
				}

				if (tval->IsObject())
					addElem(tval, sub);
				 else if (tval->IsArray()) {

					auto arr = tval->GetArray();
					u32 arrSize = (u32) arr.Size();

					if (isListIndex) {

						i64 num = sub.toLong();

						if(num < 0)
							return Log::error(OString("Couldn't mkdir \"") + cpath + "\" arrays range from [0, u32_MAX]");

						for (u32 i = arrSize; i <= num; ++i)
							addElemToArr(arr);
					}
					else
						return Log::error(OString("Couldn't mkdir \"") + cpath + "\" it didn't use an index to reference to something in an array");

				} else 
					return Log::error(OString("Couldn't mkdir \"") + cpath + "\" it was a data type, not an object or list");

			} else 
				json.AddMember(rapidjson::Value(sub.c_str(), sub.size(), json.GetAllocator()), rapidjson::Value(rapidjson::Type::kObjectType), json.GetAllocator());

		}

		++i;
	}

	return true;
}