<template>
  <div class=\"alerts-page\">
    <h2 class=\"page-title\">Alert Center</h2>
    <el-card shadow=\"hover\" style=\"margin-bottom:16px\">
      <el-form :inline=\"true\">
        <el-form-item label=\"Alert Level\">
          <el-select v-model=\"filter.level\" placeholder=\"All\" clearable style=\"width:140px\">
            <el-option label=\"Critical\" value=\"critical\"/><el-option label=\"Warning\" value=\"warning\"/><el-option label=\"Info\" value=\"info\"/>
          </el-select>
        </el-form-item>
        <el-form-item label=\"Alert Type\">
          <el-select v-model=\"filter.type\" placeholder=\"All\" clearable style=\"width:160px\">
            <el-option label=\"Phone Usage\" value=\"Phone Usage\"/><el-option label=\"Hardhat Absence\" value=\"Hardhat Absence\"/><el-option label=\"Smoking\" value=\"Smoking\"/>
          </el-select>
        </el-form-item>
        <el-form-item label=\"Processing Status\">
          <el-select v-model=\"filter.status\" placeholder=\"All\" clearable style=\"width:140px\">
            <el-option label=\"Pending\" value=\"pending\"/><el-option label=\"Confirmed\" value=\"confirmed\"/><el-option label=\"Ignored\" value=\"ignored\"/>
          </el-select>
        </el-form-item>
        <el-form-item>
          <el-button type=\"primary\" @click=\"page=1\">Search</el-button>
          <el-button @click=\"filter={level:'',type:'',status:''};page=1\">Reset</el-button>
        </el-form-item>
      </el-form>
    </el-card>
    <el-card shadow=\"hover\">
      <el-table :data=\"filteredAlerts\" stripe style=\"width:100%\" v-loading=\"loading\">
        <el-table-column prop=\"alert_id\" label=\"Alert ID\" width=\"100\"/>
        <el-table-column prop=\"timestamp\" label=\"Time\" width=\"180\"/>
        <el-table-column prop=\"channel_id\" label=\"Channel\" width=\"120\"/>
        <el-table-column prop=\"behavior_type\" label=\"Behavior Type\" width=\"140\"/>
        <el-table-column label=\"Level\" width=\"100\" align=\"center\"><template #default=\"{row}\"><el-tag :type=\"lt(row.alert_level)\" size=\"small\">{{ row.alert_level }}</el-tag></template></el-table-column>
        <el-table-column prop=\"confidence\" label=\"Confidence\" width=\"100\" align=\"center\"><template #default=\"{row}\">{{(row.confidence*100).toFixed(1)}}%</template></el-table-column>
        <el-table-column label=\"Status\" width=\"100\" align=\"center\"><template #default=\"{row}\"><el-tag :type=\"stType(row.status)\" size=\"small\">{{ stText(row.status) }}</el-tag></template></el-table-column>
        <el-table-column label=\"Actions\" width=\"200\" align=\"center\"><template #default=\"{row}\">
          <el-button link type=\"primary\" size=\"small\" @click=\"confirmAlert(row)\">Confirm</el-button>
          <el-button link type=\"info\" size=\"small\" @click=\"ignoreAlert(row)\">Ignore</el-button>
          <el-button link type=\"danger\" size=\"small\" @click=\"deleteAlert(row)\">Delete</el-button>
        </template></el-table-column>
      </el-table>
      <el-pagination v-model:current-page=\"page\" :page-size=\"10\" layout=\"prev,pager,next,total\" :total=\"filteredAlerts.length\" style=\"margin-top:16px;justify-content:flex-end\"/>
    </el-card>
  </div>
</template>
<script setup>
import { ref, computed, onMounted } from 'vue'; import { ElMessage } from 'element-plus'; import api from '@/utils/api'
const allAlerts = ref([]); const loading = ref(false); const page = ref(1)
const filter = ref({level:'',type:'',status:''})
const filteredAlerts = computed(()=>{let l=allAlerts.value;if(filter.value.level)l=l.filter(a=>a.alert_level===filter.value.level);if(filter.value.type)l=l.filter(a=>a.behavior_type===filter.value.type);if(filter.value.status)l=l.filter(a=>a.status===filter.value.status);return l})
const lt=l=>l==='critical'?'danger':l==='warning'?'warning':'info'
const stType=s=>s==='pending'?'warning':s==='confirmed'?'success':'info'
const stText=s=>({pending:'Pending',confirmed:'Confirmed',ignored:'Ignored'}[s]||s)
const load=async()=>{loading.value=true;try{allAlerts.value=await api.get('/alerts')}finally{loading.value=false}}
const confirmAlert=async(r)=>{try{await api.put('/alerts/'+r.alert_id+'/status',{status:'confirmed'});ElMessage.success('Confirmed');load()}catch{}}
const ignoreAlert=async(r)=>{try{await api.put('/alerts/'+r.alert_id+'/status',{status:'ignored'});ElMessage.success('Ignored');load()}catch{}}
const deleteAlert=async(r)=>{try{await api.delete('/alerts/'+r.alert_id);ElMessage.success('Deleted');load()}catch{}}
onMounted(load)
</script>
<style scoped>.alerts-page{max-width:1400px}.page-title{font-size:22px;font-weight:600;margin-bottom:20px}</style>
