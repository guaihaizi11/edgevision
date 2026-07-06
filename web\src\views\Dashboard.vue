<template>
  <div class=\"dashboard\">
    <h2 class=\"page-title\">Dashboard</h2>
    <el-row :gutter=\"16\" class=\"stat-row\">
      <el-col :span=\"6\">
        <el-card shadow=\"hover\" class=\"stat-card stat-channels\">
          <div class=\"stat-icon\"><el-icon :size=\"32\"><Camera /></el-icon></div>
          <div class=\"stat-info\"><div class=\"stat-value\">{{ stats.totalChannels }}</div><div class=\"stat-label\">Total Cameras</div></div>
        </el-card>
      </el-col>
      <el-col :span=\"6\">
        <el-card shadow=\"hover\" class=\"stat-card stat-online\">
          <div class=\"stat-icon\"><el-icon :size=\"32\"><Connection /></el-icon></div>
          <div class=\"stat-info\"><div class=\"stat-value\">{{ stats.onlineChannels }}</div><div class=\"stat-label\">Online Channels</div></div>
        </el-card>
      </el-col>
      <el-col :span=\"6\">
        <el-card shadow=\"hover\" class=\"stat-card stat-alerts\">
          <div class=\"stat-icon\"><el-icon :size=\"32\"><Bell /></el-icon></div>
          <div class=\"stat-info\"><div class=\"stat-value\">{{ stats.totalAlerts }}</div><div class=\"stat-label\">Total Alerts</div></div>
        </el-card>
      </el-col>
      <el-col :span=\"6\">
        <el-card shadow=\"hover\" class=\"stat-card stat-pending\">
          <div class=\"stat-icon\"><el-icon :size=\"32\"><Warning /></el-icon></div>
          <div class=\"stat-info\"><div class=\"stat-value\">{{ stats.pendingAlerts }}</div><div class=\"stat-label\">Pending Alerts</div></div>
        </el-card>
      </el-col>
    </el-row>
    <el-row :gutter=\"16\" class=\"chart-row\">
      <el-col :span=\"14\">
        <el-card shadow=\"hover\" class=\"chart-card\">
          <template #header><span>System Resource Monitor</span></template>
          <div ref=\"resourceChartRef\" class=\"chart-container\"></div>
        </el-card>
      </el-col>
      <el-col :span=\"10\">
        <el-card shadow=\"hover\" class=\"chart-card\">
          <template #header><span>Alert Distribution</span></template>
          <div ref=\"alertChartRef\" class=\"chart-container\"></div>
        </el-card>
      </el-col>
    </el-row>
    <el-row :gutter=\"16\" class=\"info-row\">
      <el-col :span=\"12\">
        <el-card shadow=\"hover\">
          <template #header><span>System Status</span></template>
          <el-descriptions :column=\"1\" border>
            <el-descriptions-item label=\"CPU Usage\"><el-progress :percentage=\"sys.cpu_usage\" :color=\"pc(sys.cpu_usage)\" :stroke-width=\"18\" /></el-descriptions-item>
            <el-descriptions-item label=\"Memory Usage\"><el-progress :percentage=\"sys.mem_usage\" :color=\"pc(sys.mem_usage)\" :stroke-width=\"18\" /></el-descriptions-item>
            <el-descriptions-item label=\"NPU Usage\"><el-progress :percentage=\"sys.npu_usage\" :color=\"pc(sys.npu_usage)\" :stroke-width=\"18\" /></el-descriptions-item>
            <el-descriptions-item label=\"NPU Temperature\">{{ sys.npu_temp }}°C</el-descriptions-item>
            <el-descriptions-item label=\"CPU Temperature\">{{ sys.cpu_temp }}°C</el-descriptions-item>
          </el-descriptions>
        </el-card>
      </el-col>
      <el-col :span=\"12\">
        <el-card shadow=\"hover\">
          <template #header><span>Recent Alerts</span></template>
          <el-timeline>
            <el-timeline-item v-for=\"a in recentAlerts\" :key=\"a.alert_id\" :timestamp=\"a.timestamp\" placement=\"top\">
              <el-tag :type=\"lt(a.alert_level)\" size=\"small\">{{ a.behavior_type }}</el-tag>
              <span style=\"margin-left:8px;color:#9ca3af\">{{ a.channel_id }}</span>
            </el-timeline-item>
          </el-timeline>
          <el-empty v-if=\"recentAlerts.length === 0\" description=\"No alerts\" :image-size=\"60\" />
        </el-card>
      </el-col>
    </el-row>
  </div>
</template>
<script setup>
import { ref, onMounted, onUnmounted } from 'vue'
import * as echarts from 'echarts'
import api from '@/utils/api'
const stats = ref({ totalChannels:0, onlineChannels:0, totalAlerts:0, pendingAlerts:0 })
const sys = ref({ cpu_usage:45, mem_usage:62, npu_usage:0, npu_temp:0, cpu_temp:0 })
const recentAlerts = ref([])
const resourceChartRef = ref(null), alertChartRef = ref(null)
let resourceTimer = null
const pc = (v) => v > 80 ? '#F56C6C' : v > 60 ? '#E6A23C' : '#67C23A'
const lt = (l) => l === 'critical' ? 'danger' : l === 'warning' ? 'warning' : 'info'
const loadStats = async () => {
  try { const a = await api.get('/alerts/stats'); stats.value.totalAlerts=a.total||0; stats.value.pendingAlerts=a.pending||0 } catch{}
  try { const c = await api.get('/channels'); stats.value.totalChannels=c.length; stats.value.onlineChannels=c.filter(ch=>ch.status==='running').length } catch{}
  try { const s = await api.get('/system/stats'); sys.value=s } catch{}
  try { const al = await api.get('/alerts'); recentAlerts.value=al.slice(0,5) } catch{}
}
const initResourceChart = () => {
  const chart = echarts.init(resourceChartRef.value)
  const opt = {
    tooltip:{trigger:'axis'}, legend:{data:['CPU','Memory','NPU'],textStyle:{color:'#9ca3af'},top:0},
    grid:{left:3,right:3,bottom:3,top:40,containLabel:true},
    xAxis:{type:'category',data:Array.from({length:20},(_,i)=>(i*3)+'s'),axisLine:{lineStyle:{color:'#374151'}},axisLabel:{color:'#9ca3af'}},
    yAxis:{type:'value',max:100,axisLine:{lineStyle:{color:'#374151'}},axisLabel:{color:'#9ca3af'},splitLine:{lineStyle:{color:'#1f2937'}}},
    series:[
      {name:'CPU',type:'line',data:Array.from({length:20},()=>Math.random()*60+20),smooth:true,symbol:'none',lineStyle:{color:'#409EFF'}},
      {name:'Memory',type:'line',data:Array.from({length:20},()=>Math.random()*30+40),smooth:true,symbol:'none',lineStyle:{color:'#67C23A'}},
      {name:'NPU',type:'line',data:Array.from({length:20},()=>Math.random()*40+10),smooth:true,symbol:'none',lineStyle:{color:'#E6A23C'}}
    ]
  }
  chart.setOption(opt)
  resourceTimer = setInterval(() => {
    opt.xAxis.data.shift(); opt.xAxis.data.push(Date.now()%60000+'ms')
    opt.series[0].data.shift(); opt.series[0].data.push(Math.random()*60+20)
    opt.series[1].data.shift(); opt.series[1].data.push(Math.random()*30+40)
    opt.series[2].data.shift(); opt.series[2].data.push(Math.random()*40+10)
    chart.setOption(opt)
  }, 3000)
}
const initAlertChart = () => {
  const chart = echarts.init(alertChartRef.value)
  chart.setOption({
    tooltip:{trigger:'item'}, legend:{orient:'vertical',left:'left',textStyle:{color:'#9ca3af'}},
    series:[{type:'pie',radius:['40%','70%'],center:['55%','50%'],
      data:[{value:12,name:'Phone Usage',itemStyle:{color:'#409EFF'}},{value:8,name:'Hardhat Absence',itemStyle:{color:'#F56C6C'}},{value:5,name:'Smoking',itemStyle:{color:'#E6A23C'}},{value:3,name:'Other',itemStyle:{color:'#909399'}}],
      label:{color:'#e5e7eb'},emphasis:{label:{show:true,fontSize:14,fontWeight:'bold'}}}]
  })
}
onMounted(()=>{loadStats();initResourceChart();initAlertChart()})
onUnmounted(()=>{if(resourceTimer)clearInterval(resourceTimer)})
</script>
<style scoped>
.dashboard{max-width:1400px}.page-title{font-size:22px;font-weight:600;margin-bottom:20px}
.stat-row{margin-bottom:16px}.stat-card{border-radius:8px}
.stat-card :deep(.el-card__body){display:flex;align-items:center;gap:16px;padding:24px}
.stat-icon{width:56px;height:56px;border-radius:12px;display:flex;align-items:center;justify-content:center}
.stat-channels .stat-icon{background:rgba(64,158,255,.15);color:#409EFF}
.stat-online .stat-icon{background:rgba(103,194,58,.15);color:#67C23A}
.stat-alerts .stat-icon{background:rgba(230,162,60,.15);color:#E6A23C}
.stat-pending .stat-icon{background:rgba(245,108,108,.15);color:#F56C6C}
.stat-value{font-size:28px;font-weight:700}.stat-label{font-size:13px;color:#9ca3af}
.chart-row{margin-bottom:16px}.chart-card{border-radius:8px}.chart-container{height:280px}
.info-row{margin-bottom:16px}
</style>
