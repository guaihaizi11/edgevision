import { defineStore } from 'pinia'
import { ref } from 'vue'
import api from '@/utils/api'

export const useAuthStore = defineStore('auth', () => {
  const token = ref(localStorage.getItem('ev_token') || '')
  const username = ref(localStorage.getItem('ev_username') || '')

  const login = async (user, pwd) => {
    // Simple demo login - in production, call backend auth API
    if (user && pwd) {
      token.value = 'demo-token-' + Date.now()
      username.value = user
      localStorage.setItem('ev_token', token.value)
      localStorage.setItem('ev_username', user)
      return true
    }
    return false
  }

  const logout = () => {
    token.value = ''
    username.value = ''
    localStorage.removeItem('ev_token')
    localStorage.removeItem('ev_username')
  }

  return { token, username, login, logout }
})
