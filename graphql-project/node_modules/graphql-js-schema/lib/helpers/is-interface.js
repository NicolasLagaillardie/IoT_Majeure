'use strict';

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.default = isInterface;
function isInterface(typeDescriptor) {
  return typeDescriptor.kind === 'INTERFACE';
}