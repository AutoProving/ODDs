// Copyright (c) 2019-2020 Vasily Alferov, Mateus de Oliveira Oliveira and Contributors. 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <ODDs/JSONDump.h>
#include <rapidjson/rapidjson.h>

#include <ODDs/ODDs.h>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <sstream>

namespace ODDs {
namespace {

template<class W>
void writeInt(W& writer, const char* key, int value) {
    writer.Key(key);
    writer.Int(value);
}

template<class W>
void writeBool(W& writer, const char* key, bool value) {
    writer.Key(key);
    writer.Bool(value);
}

template<class W>
void writeString(W& writer, const char* key, std::string value) {
    writer.Key(key);
    writer.String(value.c_str());
}

template<class W>
void writeStateSet(W& writer,
                   const char* key,
                   const ODD::StateContainer& states) {
    writer.Key(key);
    writer.StartArray();
    for (ODD::State state : states) {
        writer.Int(state);
    }
    writer.EndArray();
}

template<class W>
void writeAlphabet(W& writer,
                   const char* key,
                   const ODD::AlphabetMap& alphabet) {
    writer.Key(key);
    writer.StartArray();
    for (std::string symbol : alphabet.symbols()) {
        writer.String(symbol.c_str());
    }
    writer.EndArray();
}

template<class W>
void writeTransition(W& writer,
                     const ODD::Transition& transition,
                     const ODD::AlphabetMap& alphabet) {
    writer.StartObject();
    writeInt(writer, "from", transition.from);
    writeString(writer, "symbol", alphabet.numberToSymbol(transition.symbol));
    writeInt(writer, "to", transition.to);
    writer.EndObject();
}

template<class W>
void writeTransitionSet(W& writer,
                        const char* key,
                        const ODD::TransitionContainer& transitions,
                        const ODD::AlphabetMap& alphabet) {
    writer.Key(key);
    writer.StartArray();
    for (const ODD::Transition& transition : transitions) {
        writeTransition(writer, transition, alphabet);
    }
    writer.EndArray();
}

template<class W>
void writeLayer(const ODD::Layer& layer, W& writer) {
    const ODD::AlphabetMap& alphabet = layer.alphabet;
    writer.StartObject();
    writeAlphabet(writer, "alphabet", alphabet);
    writeTransitionSet(writer, "transitions", layer.transitions, alphabet);
    writeInt(writer, "rightLayerStates", layer.rightStates);
    writer.EndObject();
}

template<class W>
void writeLayerDetailed(const ODD::Layer& layer, W& writer) {
    const ODD::AlphabetMap& alphabet = layer.alphabet;
    writer.StartObject();
    writeAlphabet(writer, "alphabet", alphabet);
    writeInt(writer, "leftLayerStates", layer.leftStates);
    writeStateSet(writer, "initialStates", layer.initialStates);
    writeInt(writer, "rightLayerStates", layer.rightStates);
    writeStateSet(writer, "finalStates", layer.finalStates);
    writeTransitionSet(writer, "transitions", layer.transitions, alphabet);
    writeBool(writer, "isInitial", layer.isInitial);
    writeBool(writer, "isFinal", layer.isFinal);
    writer.EndObject();
}

template<class W>
void writeJSON(const ODD& odd, W& writer) {
    writer.StartObject();

    writeInt(writer, "leftLayerStates", odd.getLayer(0).leftStates);
    writeStateSet(writer, "initialStates", odd.getLayer(0).initialStates);

    writer.Key("layers");
    writer.StartArray();
    for (int i = 0; i < odd.countLayers(); i++) {
        const ODD::Layer& layer = odd.getLayer(i);
        writeLayer(layer, writer);
    }
    writer.EndArray();

    const auto& finalStates = odd.getLayer(odd.countLayers() - 1).finalStates;
    writeStateSet(writer, "finalStates", finalStates);

    writer.EndObject();
}

}

namespace {

template<class B>
void prettyprintJSON(B& buffer, const ODD& odd) {
    rapidjson::PrettyWriter<B> writer(buffer);
    writer.SetIndent(' ', 2);
    writeJSON(odd, writer);
}

}

void writeJSON(std::ostream& os, const ODD& odd) {
    rapidjson::OStreamWrapper s(os);
    prettyprintJSON(s, odd);
}

void writeJSON(std::ostream& os, const ODD::Layer& layer) {
    rapidjson::OStreamWrapper s(os);
    rapidjson::Writer<rapidjson::OStreamWrapper> w(s);
    writeLayerDetailed(layer, w);
}

std::string writeJSON(const ODD& odd) {
    rapidjson::StringBuffer s;
    prettyprintJSON(s, odd);
    return s.GetString();
}

namespace {

class JSONKeyError : public JSONParseError {
    static std::string makeMessage(const char* key, const char* type) {
        std::ostringstream os;
        os << key << ": expected " << type;
        return os.str();
    }

public:
    JSONKeyError(const char* key, const char* type)
        : JSONParseError(makeMessage(key, type))
    {}

    virtual ~JSONKeyError() = default;
};

template<class T>
int readInt(const T& object, const char* key) {
    if (!object.HasMember(key))
        throw JSONKeyError(key, "int");
    if (!object[key].IsInt())
        throw JSONKeyError(key, "int");
    return object[key].GetInt();
}

template<class T>
bool readBool(const T& object, const char* key) {
    if (!object.HasMember(key))
        throw JSONKeyError(key, "bool");
    if (!object[key].IsBool())
        throw JSONKeyError(key, "bool");
    return object[key].GetBool();
}

template<class T>
std::string readString(const T& object, const char* key) {
    if (!object.HasMember(key))
        throw JSONKeyError(key, "string");
    if (!object[key].IsString())
        throw JSONKeyError(key, "string");
    return object[key].GetString();
}

template<class T>
ODD::StateContainer readStateSet(const T& object, const char* key) {
    if (!object.HasMember(key))
        throw JSONKeyError(key, "state container");
    if (!object[key].IsArray())
        throw JSONKeyError(key, "state container");
    ODD::StateContainer states;
    for (const auto& v : object[key].GetArray()) {
        if (!v.IsInt())
            throw JSONKeyError(key, "state container");
        states.insert(v.GetInt());
    }
    return states;
}

template<class T>
ODD::AlphabetMap readAlphabetMap(const T& object, const char* key) {
    if (!object.HasMember(key))
        throw JSONKeyError(key, "alphabet map");
    if (!object[key].IsArray())
        throw JSONKeyError(key, "alphabet map");
    ODD::AlphabetMap alphabet;
    for (const auto& v : object[key].GetArray()) {
        if (!v.IsString())
            throw JSONKeyError(key, "alphabet map");
        alphabet.addSymbol(v.GetString());
    }
    return alphabet;
}

template<class T>
ODD::Transition readTransition(const T& object,
                               const char* key,
                               const ODD::AlphabetMap& alphabet) {
    if (!object.IsObject())
        throw JSONKeyError(key, "transition container");
    return {
        readInt(object, "from"),
        alphabet.symbolToNumber(readString(object, "symbol")),
        readInt(object, "to")
    };
}

template<class T>
ODD::TransitionContainer readTransitionSet(const T& object,
                                           const char* key,
                                           const ODD::AlphabetMap& alphabet) {
    if (!object.HasMember(key))
        throw JSONKeyError(key, "transition container");
    if (!object[key].IsArray())
        throw JSONKeyError(key, "transition container");
    ODD::TransitionContainer transitions;
    for (const auto& transition : object[key].GetArray()) {
        transitions.insert(readTransition(transition, key, alphabet));
    }
    return transitions;
}

template<class T>
ODD::Layer readLayerDetailed(const T& layer) {
    if (!layer.IsObject())
        throw JSONKeyError("layers", "layer");
    ODD::AlphabetMap alphabet = readAlphabetMap(layer, "alphabet");
    int leftStates = readInt(layer, "leftLayerStates");
    ODD::StateContainer initialStates = readStateSet(layer, "initialStates");
    int rightStates = readInt(layer, "rightLayerStates");
    ODD::StateContainer finalStates = readStateSet(layer, "finalStates");
    ODD::TransitionContainer transitions =
        readTransitionSet(layer, "transitions", alphabet);
    bool isInitial = readBool(layer, "isInitial");
    bool isFinal = readBool(layer, "isFinal");
    return ODD::Layer {
        std::move(alphabet),
        leftStates,
        std::move(initialStates),
        rightStates,
        std::move(finalStates),
        std::move(transitions),
        isInitial,
        isFinal
    };
}

template<class S>
ODD::Layer streamReadLayerDetailed(S& stream) {
    rapidjson::Document d;
    if (d.ParseStream(stream).HasParseError()) {
        throw JSONParseError(rapidjson::GetParseError_En(d.GetParseError()));
    }
    if (!d.IsObject()) {
        throw JSONKeyError("<document>", "object");
    }

    return readLayerDetailed<>(d);
}

template<class S>
ODD readJSON(S& stream) {
    rapidjson::Document d;
    if (d.ParseStream(stream).HasParseError()) {
        throw JSONParseError(rapidjson::GetParseError_En(d.GetParseError()));
    }
    if (!d.IsObject()) {
        throw JSONKeyError("<document>", "object");
    }

    int leftStates = readInt(d, "leftLayerStates");
    ODDBuilder builder(leftStates);

    builder.setInitialStates(readStateSet(d, "initialStates"));

    if (!d.HasMember("layers"))
        throw JSONKeyError("layers", "array");
    if (!d["layers"].IsArray())
        throw JSONKeyError("layers", "array");
    for (const auto& layer : d["layers"].GetArray()) {
        if (!layer.IsObject())
            throw JSONKeyError("layers", "layer");
        ODD::AlphabetMap alphabet = readAlphabetMap(layer, "alphabet");
        ODD::TransitionContainer transitions =
            readTransitionSet(layer, "transitions", alphabet);
        int rightStates = readInt(layer, "rightLayerStates");
        builder.addLayer(alphabet, transitions, rightStates);
    }

    builder.setFinalStates(readStateSet(d, "finalStates"));

    return builder.build();
}

}

JSONParseError::JSONParseError(std::string message)
    : std::runtime_error(message)
{}

ODD readJSON(std::istream& is) {
    rapidjson::IStreamWrapper w(is);
    return readJSON<>(w);
}

ODD::Layer readLayerJSON(std::istream& is) {
    rapidjson::IStreamWrapper w(is);
    return streamReadLayerDetailed<>(w);
}

ODD readJSON(const std::string& description) {
    rapidjson::StringStream s(description.c_str());
    return readJSON<>(s);
}

}
