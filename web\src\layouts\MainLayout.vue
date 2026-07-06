<template>
  <el-container class=\"main-container\">
    <el-aside :width=\"sidebarWidth\" class=\"sidebar\">
      <div class=\"logo-area\">
        <el-icon :size=\"28\" color=\"#409EFF\"><VideoCamera /></el-icon>
        <span v-show=\"!collapsed\" class=\"logo-text\">EdgeVision</span>
      </div>
      <el-menu :default-active=\"route.path\" router background-color=\"#0d1117\" text-color=\"#9ca3af\"
        active-text-color=\"#409EFF\" :collapse=\"collapsed\" :collapse-transition=\"false\" class=\"side-menu\">
        <el-menu-item index=\"/dashboard\"><el-icon><DataAnalysis /></el-icon><template #title>Dashboard</template></el-menu-item>
        <el-menu-item index=\"/monitor\"><el-icon><Monitor /></el-icon><template #title>Real-time Monitor</template></el-menu-item>
        <el-menu-item index=\"/cameras\"><el-icon><Camera /></el-icon><template #title>Camera Management</template></el-menu-item>
        <el-menu-item index=\"/algorithms\"><el-icon><Cpu /></el-icon><template #title>Algorithm Management</template></el-menu-item>
        <el-menu-item index=\"/behaviors\"><el-icon><Operation /></el-icon><template #title>Behavior Rules</template></el-menu-item>
        <el-menu-item index=\"/alerts\">
          <el-badge :value=\"store.unreadAlerts\" :hidden=\"store.unreadAlerts === 0\">
            <el-icon><Bell /></el-icon>
          </el-badge>
          <template #title>Alert Center</template>
        </el-menu-item>
        <el-menu-item index=\"/settings\"><el-icon><Setting /></el-icon><template #title>System Settings</template></el-menu-item>
      </el-menu>
    </el-aside>
    <el-container>
      <el-header class=\"header\">
        <div class=\"header-left\">
          <el-icon class=\"collapse-btn\" @click=\"toggleSidebar\"><Fold v-if=\"!collapsed\" /><Expand v-else /></el-icon>
          <h3>{{ currentTitle }}</h3>
        </div>
        <div class=\"header-right\">
          <el-badge :value=\"unreadCount\" :hidden=\"unreadCount === 0\">
            <el-icon @click=\"drawerVisible = true\"><Bell /></el-icon>
          </el-badge>
          <el-dropdown @command=\"handleCommand\">
            <span class=\"user-info\">
              <el-icon><User /></el-icon>
              {{ authStore.username || 'Admin' }} <el-icon><ArrowDown /></el-icon>
            </span>
            <template #dropdown>
              <el-dropdown-menu>
                <el-dropdown-item command=\"settings\">Settings</el-dropdown-item>
                <el-dropdown-item command=\"logout\" divided>Logout</el-dropdown-item>
              </el-dropdown-menu>
            </template>
          </el-dropdown>
        </div>
      </el-header>
      <el-main class=\"main-content\"><router-view /></el-main>
    </el-container>
    <el-drawer v-model=\"drawerVisible\" title=\"System Notifications\" :size=\"400\">
      <div v-if=\"store.notifications.length === 0\" style=\"text-align:center;color:#9ca3af;padding:40px 0\">No notifications</div>
      <div v-for=\"n in store.notifications\" :key=\"n.id\" class=\"notif-item\" :class=\"{ unread: !n.read }\">
        <div class=\"notif-msg\">{{ n.message }}</div><div class=\"notif-time\">{{ n.time }}</div>
      </div>
    </el-drawer>
  </el-container>
</template>
<script setup>
import { computed, ref } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import { useAppStore } from '@/stores/app'
import { useAuthStore } from '@/stores/auth'
const route = useRoute(), router = useRouter(), store = useAppStore(), authStore = useAuthStore()
const collapsed = computed(() => store.sidebarCollapsed)
const sidebarWidth = computed(() => collapsed.value ? '64px' : '220px')
const currentTitle = computed(() => route.meta.title || '')
const unreadCount = computed(() => store.notifications.filter(n => !n.read).length)
const drawerVisible = ref(false)
const toggleSidebar = () => store.toggleSidebar()
const handleCommand = (cmd) => { cmd === 'logout' ? (authStore.logout(), router.push('/login')) : cmd === 'settings' && router.push('/settings') }
</script>
<style scoped>
.main-container{height:100%}.sidebar{background:#0d1117;border-right:1px solid #1f2937;transition:width .3s;overflow:hidden}
.logo-area{height:60px;display:flex;align-items:center;justify-content:center;gap:10px;border-bottom:1px solid #1f2937}
.logo-text{font-size:18px;font-weight:700;color:#409EFF}.side-menu{border-right:none}
.header{display:flex;align-items:center;justify-content:space-between;background:#111827;border-bottom:1px solid #1f2937;padding:0 20px}
.header-left{display:flex;align-items:center;gap:16px}.collapse-btn{cursor:pointer;color:#9ca3af}.collapse-btn:hover{color:#409EFF}
.header-right{display:flex;align-items:center;gap:16px}.header-right .el-icon{cursor:pointer;color:#9ca3af}.header-right .el-icon:hover{color:#409EFF}
.user-info{display:flex;align-items:center;gap:6px;cursor:pointer;color:#9ca3af}.main-content{background:#0a0e1a;overflow-y:auto;padding:20px}
.notif-item{padding:12px 16px;border-bottom:1px solid #1f2937}.notif-item.unread{background:rgba(64,158,255,.08)}
.notif-msg{font-size:14px;margin-bottom:4px}.notif-time{font-size:12px;color:#9ca3af}
</style>
