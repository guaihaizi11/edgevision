import { defineStore } from 'pinia'
import { ref, computed } from 'vue'
import api from '@/utils/api'

export const useAppStore = defineStore('app', () => {
  const sidebarCollapsed = ref(false)
  const notifications = ref([])
  const onlineChannels = ref(0)
  const totalChannels = ref(0)
  const unreadAlerts = ref(0)

  const toggleSidebar = () => { sidebarCollapsed.value = !sidebarCollapsed.value }

  const addNotification = (msg) => {
    notifications.value.unshift({
      id: Date.now(),
      message: msg,
      time: new Date().toLocaleString(),
      read: false
    })
    if (notifications.value.length > 50) notifications.value.pop()
  }

  const markRead = (id) => {
    const n = notifications.value.find(n => n.id === id)
    if (n) n.read = true
  }

  return {
    sidebarCollapsed, notifications, onlineChannels, totalChannels, unreadAlerts,
    toggleSidebar, addNotification, markRead
  }
})
