<template>
  <div class=\"behaviors-page\">
    <h2 class=\"page-title\">Behavior Rules Management</h2>
    <el-row :gutter=\"16\">
      <el-col :span=\"8\" v-for=\"(rule,idx) in rules\" :key=\"idx\">
        <el-card shadow=\"hover\" class=\"rule-card\">
          <template #header>
            <div class=\"rule-header\">
              <span class=\"rule-name\">{{ rule.name }}</span>
              <el-switch v-model=\"rule.enabled\" active-text=\"Enable\" inactive-text=\"Disable\" @change=\"toggleRule(rule)\" />
            </div>
          </template>
          <div class=\"rule-body\">
            <p class=\"rule-desc\">{{ rule.description }}</p>
            <el-descriptions :column=\"1\" border size=\"small\" style=\"margin-top:12px\">
              <el-descriptions-item label=\"Priority\"><el-tag :type=\"rule.priority===1?'danger':rule.priority===2?'warning':'info'\" size=\"small\">{{ rule.priority===1?'High':rule.priority===2?'Medium':'Low' }}</el-tag></el-descriptions-item>
              <el-descriptions-item label=\"Cooldown\">{{ rule.cooldown }}s</el-descriptions-item>
              <el-descriptions-item label=\"Min Confidence\">{{ (rule.min_confidence*100).toFixed(0) }}%</el-descriptions-item>
              <el-descriptions-item label=\"MQTT Topic\">{{ rule.mqtt_topic }}</el-descriptions-item>
            </el-descriptions>
          </div>
          <div class=\"rule-footer\">
            <el-button link type=\"primary\" size=\"small\" @click=\"editRule(rule)\">Edit Rule</el-button>
          </div>
        </el-card>
      </el-col>
    </el-row>
  </div>
</template>
<script setup>
import { ref } from 'vue'; import { ElMessage } from 'element-plus'
const rules = ref([
  { rule_id:'behavior_phone_usage',name:'Phone Usage Detection',description:'Detects person holding a mobile phone beyond threshold',priority:2,cooldown:30,min_confidence:0.45,mqtt_topic:'edgevision/alert/phone',enabled:true },
  { rule_id:'behavior_no_hardhat',name:'Hardhat Absence',description:'Detects person without wearing a hardhat',priority:1,cooldown:30,min_confidence:0.5,mqtt_topic:'edgevision/alert/no_hardhat',enabled:true },
  { rule_id:'behavior_smoking',name:'Smoking Detection',description:'Detects person smoking behavior',priority:1,cooldown:60,min_confidence:0.45,mqtt_topic:'edgevision/alert/smoking',enabled:true }
])
const toggleRule = (r) => { ElMessage.success(r.name + ' ' + (r.enabled?'enabled':'disabled')) }
const editRule = (r) => { ElMessage.info('Editing rule: ' + r.name) }
</script>
<style scoped>.behaviors-page{max-width:1400px}.page-title{font-size:22px;font-weight:600;margin-bottom:20px}
.rule-card{border-radius:8px;margin-bottom:16px}.rule-header{display:flex;justify-content:space-between;align-items:center}
.rule-name{font-size:16px;font-weight:600}.rule-desc{color:#9ca3af;font-size:14px;margin-bottom:8px}
.rule-footer{margin-top:12px;display:flex;gap:8px;justify-content:flex-end}</style>
