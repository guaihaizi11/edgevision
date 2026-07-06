<template>
  <div class=\"monitor\">
    <h2 class=\"page-title\">Real-time Monitor</h2>
    <el-alert v-if=\"!connected\" type=\"warning\" closable show-icon style=\"margin-bottom:16px\">Connecting to cameras...</el-alert>
    <el-row :gutter=\"16\">
      <el-col :span=\"12\" v-for=\"ch in channels\" :key=\"ch.channel_id\">
        <el-card shadow=\"hover\" class=\"camera-card\">
          <template #header>
            <div class=\"cam-header\">
              <span>{{ ch.name }}</span>
              <el-tag :type=\"ch.status==='running'?'success':ch.status==='reconnecting'?'warning':'danger'\" size=\"small\">{{ st(ch.status) }}</el-tag>
            </div>
          </template>
          <div class=\"cam-preview\">
            <div v-if=\"ch.status==='running'\" class=\"cam-placeholder\">
              <el-icon :size=\"48\" color=\"#409EFF\"><VideoCamera /></el-icon>
              <p>Live Feed - {{ ch.name }}</p>
              <p class=\"cam-meta\">RTSP: {{ trunc(ch.rtsp_url) }}</p>
            </div>
            <div v-else class=\"cam-offline\"><el-icon :size=\"48\" color=\"#F56C6C\"><VideoCamera /></el-icon><p>Signal Lost</p></div>
          </div>
          <div class=\"cam-stats\">
            <el-statistic title=\"FPS\" :value=\"5\" :precision=\"1\"/>
            <el-statistic title=\"Resolution\" value=\"1920x1080\" style=\"margin-left:24px\"/>
            <el-statistic title=\"Latency\" value=\"45\" suffix=\"ms\" style=\"margin-left:24px\"/>
          </div>
        </el-card>
      </el-col>
    </el-row>
  </div>
</template>
<script setup>
import { ref, onMounted } from 'vue'; import api from '@/utils/api'
const channels = ref([]); const connected = ref(false)
const st = s => ({running:'Running',stopped:'Stopped',reconnecting:'Reconnecting',error:'Error'}[s]||s)
const trunc = u => u.length > 40 ? u.slice(0,37)+'...' : u
const load = async () => { try { channels.value = await api.get('/channels'); connected.value = channels.value.some(c=>c.status==='running') } catch {} }
onMounted(()=>{load();setInterval(load,10000)})
</script>
<style scoped>
.monitor{max-width:1400px}.page-title{font-size:22px;font-weight:600;margin-bottom:20px}
.camera-card{border-radius:8px}.cam-header{display:flex;justify-content:space-between;align-items:center}
.cam-preview{height:240px;display:flex;flex-direction:column;align-items:center;justify-content:center;background:#0d1117;border-radius:8px;margin-bottom:12px}
.cam-placeholder .el-icon{margin-bottom:12px}.cam-placeholder p{color:#9ca3af;font-size:14px}.cam-meta{font-size:12px;color:#4b5563!important;margin-top:4px!important}
.cam-offline .el-icon{margin-bottom:12px}.cam-offline p{color:#F56C6C;font-size:14px}
.cam-stats{display:flex;align-items:center}
</style>
