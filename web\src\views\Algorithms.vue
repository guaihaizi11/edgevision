<template>
  <div class=\"algo-page\">
    <h2 class=\"page-title\">Algorithm Management</h2>
    <div style=\"display:flex;justify-content:flex-end;margin-bottom:16px\">
      <el-button type=\"primary\" @click=\"showUpload=true\"><el-icon><Upload /></el-icon> Upload Model</el-button>
    </div>
    <el-card shadow=\"hover\">
      <el-table :data=\"models\" stripe>
        <el-table-column prop=\"model_id\" label=\"Model ID\" width=\"120\"/>
        <el-table-column prop=\"name\" label=\"Model Name\" width=\"160\"/>
        <el-table-column prop=\"file_path\" label=\"File Path\" min-width=\"200\" show-overflow-tooltip/>
        <el-table-column label=\"Input Size\" width=\"140\" align=\"center\"><template #default=\"{row}\">{{row.input_width}}x{{row.input_height}}</template></el-table-column>
        <el-table-column prop=\"quantization\" label=\"Quantization\" width=\"100\" align=\"center\"/>
        <el-table-column label=\"Conf Threshold\" width=\"120\" align=\"center\"><template #default=\"{row}\">{{(row.conf_threshold*100).toFixed(0)}}%</template></el-table-column>
        <el-table-column label=\"Status\" width=\"100\" align=\"center\"><template #default=\"{row}\"><el-tag :type=\"row.active?'success':'info'\" size=\"small\">{{row.active?'Active':'Inactive'}}</el-tag></template></el-table-column>
        <el-table-column label=\"Actions\" width=\"200\" align=\"center\"><template #default=\"{row}\">
          <el-button v-if=\"!row.active\" link type=\"primary\" size=\"small\" @click=\"activate(row)\">Activate</el-button>
          <el-button link type=\"danger\" size=\"small\" @click=\"remove(row)\">Delete</el-button>
        </template></el-table-column>
      </el-table>
    </el-card>
    <el-dialog v-model=\"showUpload\" title=\"Upload Model File\" width=\"500px\">
      <el-upload drag action=\"/api/models/upload\" :limit=\"1\" accept=\".rknn,.onnx,.bin\">
        <el-icon :size=\"48\" color=\"#409EFF\"><Upload /></el-icon>
        <div class=\"el-upload__text\">Drag file here or click to upload</div>
        <template #tip><div class=\"el-upload__tip\">Supports .rknn, .onnx, .bin formats</div></template>
      </el-upload>
      <el-form label-width=\"100px\" style=\"margin-top:16px\">
        <el-form-item label=\"Model Name\"><el-input v-model=\"newModel.name\" placeholder=\"YOLOv8n-rknn\"/></el-form-item>
        <el-form-item label=\"Quantization\"><el-select v-model=\"newModel.quantization\" style=\"width:100%\"><el-option label=\"INT8\" value=\"int8\"/><el-option label=\"FP16\" value=\"fp16\"/></el-select></el-form-item>
      </el-form>
      <template #footer><el-button @click=\"showUpload=false\">Cancel</el-button><el-button type=\"primary\" @click=\"uploadModel\">Upload</el-button></template>
    </el-dialog>
  </div>
</template>
<script setup>
import { ref, reactive } from 'vue'; import { ElMessage } from 'element-plus'
const models = ref([
  { model_id:'m001',name:'YOLOv8n-RK3588',file_path:'/models/yolov8n.rknn',input_width:640,input_height:640,conf_threshold:0.45,iou_threshold:0.45,quantization:'int8',active:true,created_at:'2026-07-01' },
  { model_id:'m002',name:'YOLOv11n-RK3588',file_path:'/models/yolov11n.rknn',input_width:640,input_height:640,conf_threshold:0.45,iou_threshold:0.45,quantization:'int8',active:false,created_at:'2026-07-02' }
])
const showUpload = ref(false); const newModel = reactive({name:'',quantization:'int8'})
const activate = (m) => { models.value.forEach(x=>x.active=false); m.active=true; ElMessage.success('Model activated'); }
const remove = (m) => { ElMessage.success('Deleted'); models.value=models.value.filter(x=>x.model_id!==m.model_id) }
const uploadModel = () => { ElMessage.success('Model uploaded successfully'); showUpload.value=false; }
</script>
<style scoped>.algo-page{max-width:1400px}.page-title{font-size:22px;font-weight:600;margin-bottom:20px}
.el-upload__tip{color:#9ca3af;font-size:12px}</style>
