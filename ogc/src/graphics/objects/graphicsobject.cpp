#include "graphics/graphics.h"
#include "graphics/objects/graphicsobject.h"
using namespace oi::gc;
using namespace oi;

std::unordered_map<size_t, String> GraphicsObject::names = std::unordered_map<size_t, String>();

GraphicsObject::~GraphicsObject() { g->remove(this); }
size_t GraphicsObject::getTypeId() const { return typeId; }
u32 GraphicsObject::getId() const { return id; }
String GraphicsObject::getTypeName() const { return names[typeId]; }
String GraphicsObject::getName() const { return name; }
i32 GraphicsObject::getRefCount() const { return refCount; }
