'use strict';

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.default = isNotIntrospectionType;
function isNotIntrospectionType(typeDescriptor) {
  return !typeDescriptor.name.startsWith('__');
}