'use strict';

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.default = isScalar;
function isScalar(typeDescriptor) {
  return typeDescriptor.kind === 'OBJECT';
}