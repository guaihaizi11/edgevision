<template>
  <div class="login-page">
    <div class="login-card">
      <div class="login-header">
        <el-icon :size="48" color=\"#409EFF\"><VideoCamera /></el-icon>
        <h1>EdgeVision</h1>
        <p>Video Behavior Analysis System</p>
      </div>
      <el-form :model=\"form\" @submit.prevent=\"handleLogin\" class=\"login-form\">
        <el-form-item>
          <el-input v-model=\"form.username\" placeholder=\"Username\" :prefix-icon=\"User\" size=\"large\" />
        </el-form-item>
        <el-form-item>
          <el-input v-model=\"form.password\" type=\"password\" placeholder=\"Password\" :prefix-icon=\"Lock\" size=\"large\" show-password />
        </el-form-item>
        <el-form-item>
          <el-button type=\"primary\" size=\"large\" :loading=\"loading\" style=\"width:100%\" @click=\"handleLogin\">Login</el-button>
        </el-form-item>
      </el-form>
      <div class=\"login-footer\">EdgeVision v1.0.0 &copy; 2026</div>
    </div>
  </div>
</template>
<script setup>
import { ref, reactive } from 'vue'
import { useRouter } from 'vue-router'
import { useAuthStore } from '@/stores/auth'
import { User, Lock, VideoCamera } from '@element-plus/icons-vue'
import { ElMessage } from 'element-plus'
const router = useRouter(), authStore = useAuthStore()
const loading = ref(false)
const form = reactive({ username: '', password: '' })
const handleLogin = async () => {
  if (!form.username || !form.password) { ElMessage.warning('Please enter username and password'); return }
  loading.value = true
  try {
    const ok = await authStore.login(form.username, form.password)
    if (ok) { ElMessage.success('Login successful'); router.push('/') }
  } finally { loading.value = false }
}
</script>
<style scoped>
.login-page{width:100%;height:100%;display:flex;align-items:center;justify-content:center;background:linear-gradient(135deg,#0a0e1a 0%,#1a1f3a 100%)}
.login-card{width:420px;padding:48px 40px;background:#111827;border-radius:12px;border:1px solid #1f2937;box-shadow:0 20px 60px rgba(0,0,0,.5)}
.login-header{text-align:center;margin-bottom:36px}.login-header h1{color:#409EFF;font-size:28px;margin:12px 0 4px}
.login-header p{color:#9ca3af;font-size:14px}.login-form .el-form-item{margin-bottom:20px}
.login-footer{text-align:center;color:#4b5563;font-size:12px;margin-top:24px}
</style>
