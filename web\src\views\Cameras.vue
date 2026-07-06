<template>
  <div class=\"cameras-page\">
    <div class=\"page-header\">
      <h2 class=\"page-title\">Camera Management</h2>
      <el-button type=\"primary\" @click=\"showDialog()\"><el-icon><Plus /></el-icon> Add Channel</el-button>
    </div>
    <el-card shadow=\"hover\">
      <el-table :data=\"channels\" stripe style=\"width:100%\" v-loading=\"loading\">
        <el-table-column prop=\"channel_id\" label=\"Channel ID\" width=\"120\" />
        <el-table-column prop=\"name\" label=\"Name\" min-width=\"160\" />
        <el-table-column prop=\"rtsp_url\" label=\"RTSP URL\" min-width=\"280\" show-overflow-tooltip />
        <el-table-column prop=\"fps\" label=\"FPS\" width=\"80\" align=\"center\" />
        <el-table-column label=\"Status\" width=\"100\" align=\"center\">
          <template #default=\"{row}\"><el-tag :type=\"row.status==='running'?'success':row.status==='reconnecting'?'warning':'danger'\" size=\"small\">{{ statusText(row.status) }}</el-tag></template>
        </el-table-column>
        <el-table-column label=\"Enabled\" width=\"80\" align=\"center\">
          <template #default=\"{row}\"><el-switch v-model=\"row.enabled\" @change=\"toggleChannel(row)\" /></template>
        </el-table-column>
        <el-table-column label=\"Actions\" width=\"160\" align=\"center\">
          <template #default=\"{row}\"><el-button link type=\"primary\" size=\"small\" @click=\"showDialog(row)\">Edit</el-button><el-button link type=\"danger\" size=\"small\" @click=\"deleteChannel(row)\">Delete</el-button></template>
        </el-table-column>
      </el-table>
    </el-card>
    <el-dialog v-model=\"dialogVisible\" :title=\"editing?'Edit Channel':'Add Channel'\" width=\"560px\" destroy-on-close>
      <el-form :model=\"form\" label-width=\"100px\">
        <el-row :gutter=\"16\"><el-col :span=\"12\"><el-form-item label=\"Channel ID\"><el-input v-model=\"form.channel_id\" placeholder=\"CAM-001\"/></el-form-item></el-col><el-col :span=\"12\"><el-form-item label=\"Name\"><el-input v-model=\"form.name\" placeholder=\"Construction Site Camera\"/></el-form-item></el-col></el-row>
        <el-form-item label=\"RTSP URL\"><el-input v-model=\"form.rtsp_url\" placeholder=\"rtsp://admin:pass@ip:554/stream1\"/></el-form-item>
        <el-row :gutter=\"16\"><el-col :span=\"12\"><el-form-item label=\"Username\"><el-input v-model=\"form.username\" placeholder=\"Optional\"/></el-form-item></el-col><el-col :span=\"12\"><el-form-item label=\"Password\"><el-input v-model=\"form.password\" type=\"password\" placeholder=\"Optional\" show-password/></el-form-item></el-col></el-row>
        <el-form-item label=\"Analysis FPS\"><el-slider v-model=\"form.fps\" :min=\"1\" :max=\"30\" show-input/></el-form-item>
        <el-form-item label=\"Enabled\"><el-switch v-model=\"form.enabled\"/></el-form-item>
      </el-form>
      <template #footer><el-button @click=\"dialogVisible=false\">Cancel</el-button><el-button type=\"primary\" @click=\"saveChannel\">Save</el-button></template>
    </el-dialog>
  </div>
</template>
<script setup>
import { ref, reactive, onMounted } from 'vue'; import { ElMessage, ElMessageBox } from 'element-plus'; import api from '@/utils/api'
const channels = ref([]); const loading = ref(false); const dialogVisible = ref(false); const editing = ref(null)
const form = reactive({ channel_id:'',name:'',rtsp_url:'',username:'',password:'',fps:5,enabled:true })
const statusText = s => ({running:'Running',stopped:'Stopped',reconnecting:'Reconnecting',error:'Error'}[s]||s)
const load = async () => { loading.value=true; try { channels.value=await api.get('/channels') } finally { loading.value=false } }
const showDialog = (row) => { if(row){editing.value=row.channel_id;Object.assign(form,row)}else{editing.value=null;Object.assign(form,{channel_id:'',name:'',rtsp_url:'',username:'',password:'',fps:5,enabled:true})} dialogVisible.value=true }
const saveChannel = async () => { try { if(editing.value){await api.put('/channels/'+editing.value,form)}else{await api.post('/channels',form)} ElMessage.success('Saved successfully'); dialogVisible.value=false; load() } catch(e){ElMessage.error('Operation failed')} }
const toggleChannel = async (row) => { try { await api.patch('/channels/'+row.channel_id+'/status',{enabled:row.enabled}); ElMessage.success('Status updated') } catch{} }
const deleteChannel = async (row) => { try { await ElMessageBox.confirm('Confirm delete channel: '+row.name+'?', 'Confirm', {type:'warning'}); await api.delete('/channels/'+row.channel_id); ElMessage.success('Deleted'); load() } catch{} }
onMounted(load)
</script>
<style scoped>.cameras-page{max-width:1400px}.page-header{display:flex;justify-content:space-between;align-items:center;margin-bottom:20px}.page-title{font-size:22px;font-weight:600}</style>
