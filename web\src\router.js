import { createRouter, createWebHashHistory } from 'vue-router'

const routes = [
  {
    path: '/login',
    name: 'Login',
    component: () => import('@/views/Login.vue'),
    meta: { public: true }
  },
  {
    path: '/',
    component: () => import('@/layouts/MainLayout.vue'),
    redirect: '/dashboard',
    children: [
      {
        path: 'dashboard',
        name: 'Dashboard',
        component: () => import('@/views/Dashboard.vue'),
        meta: { title: 'Dashboard' }
      },
      {
        path: 'monitor',
        name: 'Monitor',
        component: () => import('@/views/Monitor.vue'),
        meta: { title: 'Real-time Monitor' }
      },
      {
        path: 'cameras',
        name: 'Cameras',
        component: () => import('@/views/Cameras.vue'),
        meta: { title: 'Camera Management' }
      },
      {
        path: 'algorithms',
        name: 'Algorithms',
        component: () => import('@/views/Algorithms.vue'),
        meta: { title: 'Algorithm Management' }
      },
      {
        path: 'behaviors',
        name: 'Behaviors',
        component: () => import('@/views/Behaviors.vue'),
        meta: { title: 'Behavior Rules' }
      },
      {
        path: 'alerts',
        name: 'Alerts',
        component: () => import('@/views/Alerts.vue'),
        meta: { title: 'Alert Center' }
      },
      {
        path: 'settings',
        name: 'Settings',
        component: () => import('@/views/Settings.vue'),
        meta: { title: 'System Settings' }
      }
    ]
  }
]

const router = createRouter({
  history: createWebHashHistory(),
  routes
})

router.beforeEach((to, from, next) => {
  const token = localStorage.getItem('ev_token')
  if (!to.meta.public && !token && to.path !== '/login') {
    next('/login')
  } else if (to.path === '/login' && token) {
    next('/')
  } else {
    next()
  }
})

export default router
