<template>
  <router-view />
</template>

<script setup>
</script>

<style>
:root {
  --ev-primary: #409EFF;
  --ev-bg-dark: #0a0e1a;
  --ev-bg-card: #111827;
  --ev-bg-sidebar: #0d1117;
  --ev-text-primary: #e5e7eb;
  --ev-text-secondary: #9ca3af;
  --ev-border: #1f2937;
  --ev-success: #67C23A;
  --ev-warning: #E6A23C;
  --ev-danger: #F56C6C;
  --ev-info: #909399;
}

* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

html, body, #app {
  width: 100%;
  height: 100%;
  font-family: 'Helvetica Neue', Helvetica, 'PingFang SC', 'Hiragino Sans GB', 'Microsoft YaHei', Arial, sans-serif;
  background: var(--ev-bg-dark);
  color: var(--ev-text-primary);
  overflow: hidden;
}

/* Override Element Plus dark theme */
.el-button--primary {
  --el-button-bg-color: var(--ev-primary);
  --el-button-border-color: var(--ev-primary);
}

::-webkit-scrollbar {
  width: 6px;
  height: 6px;
}
::-webkit-scrollbar-thumb {
  background: #374151;
  border-radius: 3px;
}
::-webkit-scrollbar-track {
  background: transparent;
}
</style>
