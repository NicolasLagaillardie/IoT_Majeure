'use strict';

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.default = implementsNode;
function implementsNode(typeFromSchema) {
  const interfaces = typeFromSchema.interfaces || [];

  return interfaces.some(graphInterface => {
    return graphInterface.name === 'Node';
  });
}