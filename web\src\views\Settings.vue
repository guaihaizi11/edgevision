<template>
  <div class=\"settings-page\">
    <h2 class=\"page-title\">System Settings</h2>
    <el-tabs v-model=\"activeTab\" type=\"card\">
      <el-tab-pane label=\"MQTT Configuration\" name=\"mqtt\">
        <el-card shadow=\"hover\">
          <el-form :model=\"mqttForm\" label-width=\"140px\" style=\"max-width:600px\">
            <el-form-item label=\"Broker Address\"><el-input v-model=\"mqttForm.broker\" placeholder=\"127.0.0.1\"/></el-form-item>
            <el-form-item label=\"Port\"><el-input-number v-model=\"mqttForm.port\" :min=\"1\" :max=\"65535\" style=\"width:200px\"/></el-form-item>
            <el-form-item label=\"Username\"><el-input v-model=\"mqttForm.username\" placeholder=\"Optional\"/></el-form-item>
            <el-form-item label=\"Password\"><el-input v-model=\"mqttForm.password\" type=\"password\" show-password placeholder=\"Optional\"/></el-form-item>
            <el-form-item label=\"Client ID\"><el-input v-model=\"mqttForm.client_id\" placeholder=\"edgevision\"/></el-form-item>
            <el-form-item label=\"QoS\"><el-radio-group v-model=\"mqttForm.qos\"><el-radio :value=\"0\">0</el-radio><el-radio :value=\"1\">1</el-radio><el-radio :value=\"2\">2</el-radio></el-radio-group></el-form-item>
            <el-form-item label=\"Keep Alive\"><el-input-number v-model=\"mqttForm.keep_alive\" :min=\"10\" :max=\"3600\" style=\"width:200px\"/><span style=\"margin-left:8px;color:#9ca3af\">s</span></el-form-item>
            <el-form-item><el-button type=\"primary\" @click=\"saveMqtt\">Save Configuration</el-button><el-button @click=\"testMqtt\">Test Connection</el-button></el-form-item>
          </el-form>
        </el-card>
      </el-tab-pane>
      <el-tab-pane label=\"HTTP Callback\" name=\"http\">
        <el-card shadow=\"hover\">
          <el-form :model=\"httpForm\" label-width=\"140px\" style=\"max-width:600px\">
            <el-form-item label=\"Enable Callback\"><el-switch v-model=\"httpForm.enabled\"/></el-form-item>
            <el-form-item label=\"Callback URL\"><el-input v-model=\"httpForm.url\" placeholder=\"https://your-server.com/callback\"/></el-form-item>
            <el-form-item label=\"Timeout\"><el-input-number v-model=\"httpForm.timeout_ms\" :min=\"1000\" :max=\"30000\" style=\"width:200px\"/><span style=\"margin-left:8px;color:#9ca3af\">ms</span></el-form-item>
            <el-form-item label=\"Retry Count\"><el-input-number v-model=\"httpForm.retry_count\" :min=\"0\" :max=\"10\" style=\"width:200px\"/></el-form-item>
            <el-form-item><el-button type=\"primary\" @click=\"saveHttp\">Save Configuration</el-button></el-form-item>
          </el-form>
        </el-card>
      </el-tab-pane>
      <el-tab-pane label=\"System Monitor\" name=\"monitor\">
        <el-card shadow=\"hover\">
          <el-descriptions :column=\"2\" border>
            <el-descriptions-item label=\"CPU Usage\"><el-progress :percentage=\"mon.cpu\" :color=\"mon.cpu>80?'#F56C6C':'#67C23A'\" :stroke-width=\"20\"/></el-descriptions-item>
            <el-descriptions-item label=\"Memory Usage\"><el-progress :percentage=\"mon.mem\" :color=\"mon.mem>85?'#F56C6C':'#67C23A'\" :stroke-width=\"20\"/></el-descriptions-item>
            <el-descriptions-item label=\"NPU Usage\"><el-progress :percentage=\"mon.npu\" :color=\"mon.npu>80?'#F56C6C':'#67C23A'\" :stroke-width=\"20\"/></el-descriptions-item>
            <el-descriptions-item label=\"NPU Temperature\"><span :style=\"{color:mon.npuTemp>75?'#F56C6C':mon.npuTemp>60?'#E6A23C':'#67C23A'}\">{{mon.npuTemp}}°C</span></el-descriptions-item>
            <el-descriptions-item label=\"CPU Temperature\"><span :style=\"{color:mon.cpuTemp>80?'#F56C6C':'#67C23A'}\">{{mon.cpuTemp}}°C</span></el-descriptions-item>
            <el-descriptions-item label=\"Disk Usage\"><el-progress :percentage=\"mon.disk\" :color=\"mon.disk>80?'#F56C6C':'#67C23A'\" :stroke-width=\"20\"/></el-descriptions-item>
          </el-descriptions>
          <div style=\"margin-top:16px;text-align:center\"><el-button type=\"primary\" @click=\"refreshMon\" :loading=\"monLoading\">Refresh Data</el-button></div>
        </el-card>
      </el-tab-pane>
      <el-tab-pane label=\"Storage Management\" name=\"storage\">
        <el-card shadow=\"hover\">
          <el-form label-width=\"140px\" style=\"max-width:600px\">
            <el-form-item label=\"Snapshot Path\"><el-input v-model=\"stor.snapshot_path\" placeholder=\"./data/alerts\"/></el-form-item>
            <el-form-item label=\"Database Path\"><el-input v-model=\"stor.db_path\" placeholder=\"./data/edgevision.db\"/></el-form-item>
            <el-form-item label=\"Log Level\"><el-select v-model=\"stor.log_level\" style=\"width:200px\"><el-option label=\"DEBUG\" value=\"debug\"/><el-option label=\"INFO\" value=\"info\"/><el-option label=\"WARNING\" value=\"warning\"/><el-option label=\"ERROR\" value=\"error\"/></el-select></el-form-item>
            <el-form-item label=\"Log Path\"><el-input v-model=\"stor.log_path\" placeholder=\"./logs\"/></el-form-item>
            <el-form-item label=\"Alert Cooldown\"><el-input-number v-model=\"stor.cooldown\" :min=\"5\" :max=\"300\" style=\"width:200px\"/><span style=\"margin-left:8px;color:#9ca3af\">s</span></el-form-item>
            <el-form-item label=\"Snapshot Quality\"><el-slider v-model=\"stor.quality\" :min=\"50\" :max=\"100\" show-input style=\"width:400px\"/></el-form-item>
            <el-form-item><el-button type=\"primary\" @click=\"saveStorage\">Save Configuration</el-button></el-form-item>
          </el-form>
        </el-card>
      </el-tab-pane>
    </el-tabs>
  </div>
</template>
<script setup>
import { ref, reactive, onMounted } from 'vue'; import { ElMessage } from 'element-plus'
const activeTab = ref('mqtt')
const mqttForm = reactive({broker:'127.0.0.1',port:1883,username:'',password:'',client_id:'edgevision',qos:1,keep_alive:60})
const httpForm = reactive({enabled:false,url:'',timeout_ms:5000,retry_count:3})
const mon = reactive({cpu:45,mem:62,npu:0,npuTemp:42,cpuTemp:38,disk:35})
const monLoading = ref(false)
const stor = reactive({snapshot_path:'./data/alerts',db_path:'./data/edgevision.db',log_level:'info',log_path:'./logs',cooldown:30,quality:85})
const saveMqtt = () => ElMessage.success('MQTT configuration saved')
const saveHttp = () => ElMessage.success('HTTP callback configuration saved')
const saveStorage = () => ElMessage.success('Storage configuration saved')
const testMqtt = () => ElMessage.info('Testing MQTT connection...')
const refreshMon = () => { monLoading.value=true; setTimeout(()=>{monLoading.value=false; ElMessage.success('Data refreshed')},1000) }
onMounted(()=>{setInterval(()=>{mon.cpu=Math.round(Math.random()*30+30);mon.mem=Math.round(Math.random()*20+50);mon.npu=Math.round(Math.random()*50);mon.npuTemp=Math.round(Math.random()*15+35);mon.cpuTemp=Math.round(Math.random()*10+30);mon.disk=Math.round(Math.random()*10+30)},5000)})
</script>
<style scoped>.settings-page{max-width:1400px}.page-title{font-size:22px;font-weight:600;margin-bottom:20px}</style>
