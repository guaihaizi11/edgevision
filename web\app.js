// DEBUG LOGS START
console.log('[EdgeVision] app.js loaded');
console.log('[EdgeVision] Vue=', typeof Vue);
console.log('[EdgeVision] VueRouter=', typeof VueRouter);
console.log('[EdgeVision] ElementPlus=', typeof ElementPlus);
console.log('[EdgeVision] ElementPlusIconsVue=', typeof ElementPlusIconsVue);
console.log('[EdgeVision] axios=', typeof axios);
console.log('[EdgeVision] Pinia=', typeof Pinia);
console.log('[EdgeVision] app.js version: 2.0-complete');
// DEBUG LOGS END

// EdgeVision Frontend SPA - Fixed version
const { createApp, ref, reactive, computed, onMounted, nextTick } = Vue;
const { createRouter, createWebHashHistory, useRouter, useRoute } = VueRouter;
const { createPinia } = Pinia;
const { ElMessage, ElMessageBox } = ElementPlus;
const icons = ElementPlusIconsVue;

const api = {
  get: async (path) => {
    const token = localStorage.getItem('ev_token');
    const headers = { 'Content-Type': 'application/json' };
    if (token) headers['Authorization'] = 'Bearer ' + token;
    const url = path.startsWith('http') ? path : '/api' + (path.startsWith('/') ? path : '/' + path);
    const resp = await fetch(url, { headers });
    if (!resp.ok) throw new Error('HTTP ' + resp.status);
    return resp.json();
  },
  post: async (path, data) => {
    const token = localStorage.getItem('ev_token');
    const headers = { 'Content-Type': 'application/json' };
    if (token) headers['Authorization'] = 'Bearer ' + token;
    const resp = await fetch(path.startsWith('http') ? path : '/api' + (path.startsWith('/') ? path : '/' + path), {
      method: 'POST', headers, body: JSON.stringify(data)
    });
    if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
    return resp.json();
  },
  put: async (path, data) => {
    const token = localStorage.getItem('ev_token');
    const headers = { 'Content-Type': 'application/json' };
    if (token) headers['Authorization'] = 'Bearer ' + token;
    const resp = await fetch(path.startsWith('http') ? path : '/api' + (path.startsWith('/') ? path : '/' + path), {
      method: 'PUT', headers, body: JSON.stringify(data)
    });
    if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
    return resp.json();
  },
  delete: async (path) => {
    const token = localStorage.getItem('ev_token');
    const headers = { 'Content-Type': 'application/json' };
    if (token) headers['Authorization'] = 'Bearer ' + token;
    const resp = await fetch(path.startsWith('http') ? path : '/api' + (path.startsWith('/') ? path : '/' + path), {
      method: 'DELETE', headers
    });
    if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
    return resp.json();
  }
};

const useAuth = () => {
  const token = ref(localStorage.getItem('ev_token') || '');
  const username = ref(localStorage.getItem('ev_username') || '');
  const login = async (user, pwd) => {
    if (!user || !pwd) { ElMessage.warning('Please enter username and password'); return false; }
    token.value = 'ev-' + Date.now();
    username.value = user;
    localStorage.setItem('ev_token', token.value);
    localStorage.setItem('ev_username', user);
    return true;
  };
  const logout = () => { token.value = ''; username.value = ''; localStorage.removeItem('ev_token'); localStorage.removeItem('ev_username'); };
  return { token, username, login, logout };
};

const LoginView = {
  template: `
    <div class="login-page" style="display:flex;align-items:center;justify-content:center;height:100vh;background:linear-gradient(135deg,#0a0e1a,#1a1f35)">
      <div style="background:#111827;border:1px solid #1f2937;border-radius:12px;padding:40px;width:400px;box-shadow:0 8px 32px rgba(0,0,0,.3)">
        <div style="text-align:center;margin-bottom:30px">
          <div style="font-size:48px;color:#409EFF">[Camera]</div>
          <h1 style="color:#409EFF;font-size:28px;margin:12px 0 4px">EdgeVision</h1>
          <p style="color:#9ca3af;font-size:14px">Video Behavior Analysis System</p>
        </div>
        <form @submit.prevent="handleLogin">
          <div style="margin-bottom:16px">
            <input style="width:100%;padding:12px 16px;background:#0a0e1a;border:1px solid #1f2937;border-radius:6px;color:#e5e7eb;font-size:14px;outline:none" v-model="form.username" placeholder="Username" @keyup.enter="handleLogin" />
          </div>
          <div style="margin-bottom:16px">
            <input style="width:100%;padding:12px 16px;background:#0a0e1a;border:1px solid #1f2937;border-radius:6px;color:#e5e7eb;font-size:14px;outline:none" v-model="form.password" type="password" placeholder="Password" @keyup.enter="handleLogin" />
          </div>
          <div style="margin-bottom:16px">
            <button type="submit" style="width:100%;padding:12px;background:#409EFF;border:none;border-radius:6px;color:white;font-size:16px;cursor:pointer;opacity:1" @click="handleLogin" :disabled="loading">Login</button>
          </div>
        </form>
        <div style="text-align:center;color:#4b5563;font-size:12px;margin-top:20px">EdgeVision v1.0.0 (C) 2026</div>
      </div>
    </div>
  `,
  setup() {
    const auth = useAuth();
    const router = useRouter();
    const loading = ref(false);
    const form = reactive({ username: '', password: '' });
    const handleLogin = async () => {
      if (!form.username || !form.password) { ElMessage.warning('Please enter username and password'); return; }
      loading.value = true;
      try {
        const ok = await auth.login(form.username, form.password);
        if (ok) { ElMessage.success('Login successful'); router.push('/dashboard'); }
      } finally { loading.value = false; }
    };
    return { form, loading, handleLogin };
  }
};

const DashboardView = {
  template: `
    <div>
      <h2 style="font-size:22px;font-weight:600;margin-bottom:20px">Dashboard</h2>
      <el-row :gutter="16" style="margin-bottom:20px">
        <el-col :span="6"><el-card shadow="hover"><div style="text-align:center"><div style="font-size:32px;font-weight:bold;color:#409EFF">{{stats.channels}}</div><div style="color:#9ca3af">Total Cameras</div></div></el-card></el-col>
        <el-col :span="6"><el-card shadow="hover"><div style="text-align:center"><div style="font-size:32px;font-weight:bold;color:#67C23A">{{stats.online}}</div><div style="color:#9ca3af">Online</div></div></el-card></el-col>
        <el-col :span="6"><el-card shadow="hover"><div style="text-align:center"><div style="font-size:32px;font-weight:bold;color:#E6A23C">{{stats.alerts}}</div><div style="color:#9ca3af">Alerts</div></div></el-card></el-col>
        <el-col :span="6"><el-card shadow="hover"><div style="text-align:center"><div style="font-size:32px;font-weight:bold;color:#F56C6C">{{stats.cpu}}%</div><div style="color:#9ca3af">CPU Usage</div></div></el-card></el-col>
      </el-row>
      <el-row :gutter="16">
        <el-col :span="12"><el-card><div slot="header"><span>System Status</span></div><div v-if="sys">CPU: {{sys.cpu_usage}}% | Mem: {{sys.mem_usage}}% | NPU: {{sys.npu_usage}}%</div><div v-else>Loading...</div></el-card></el-col>
        <el-col :span="12"><el-card><div slot="header"><span>Recent Alerts</span></div><div v-for="a in recentAlerts" :key="a.alert_id" style="padding:8px 0;border-bottom:1px solid #1f2937">{{a.behavior_type}} - {{a.channel_id}}</div><div v-if="recentAlerts.length===0">No alerts</div></el-card></el-col>
      </el-row>
    </div>
  `,
  setup() {
    const stats = ref({ channels: 0, online: 0, alerts: 0, cpu: 0 });
    const sys = ref(null);
    const recentAlerts = ref([]);
    const load = async () => {
      try {
        const [ch, al, st] = await Promise.all([api.get('/channels'), api.get('/alerts'), api.get('/system/stats')]);
        stats.value.channels = ch.length;
        stats.value.online = ch.filter(c => c.status === 'running').length;
        stats.value.alerts = al.length;
        recentAlerts.value = al.slice(0, 5);
        sys.value = st;
      } catch(e) { console.error('Dashboard error:', e); }
    };
    onMounted(load);
    return { stats, sys, recentAlerts };
  }
};

const CamerasView = {
  template: `
    <div>
      <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:20px">
        <h2 style="font-size:22px;font-weight:600;margin:0">Camera Management</h2>
        <el-button type="primary" @click="showAdd">Add Camera</el-button>
      </div>
      <el-table :data="channels" stripe style="width:100%">
        <el-table-column prop="channel_id" label="ID" width="120" />
        <el-table-column prop="name" label="Name" />
        <el-table-column prop="rtsp_url" label="RTSP URL" />
        <el-table-column prop="fps" label="FPS" width="80" />
        <el-table-column prop="status" label="Status" width="100">
          <template #default="scope">
            <el-tag :type="scope.row.status==='running'?'success':scope.row.status==='stopped'?'info':'danger'">{{scope.row.status}}</el-tag>
          </template>
        </el-table-column>
        <el-table-column label="Actions" width="200">
          <template #default="scope">
            <el-button size="small" @click="editChannel(scope.row)">Edit</el-button>
            <el-button size="small" type="danger" @click="delChannel(scope.row.channel_id)">Delete</el-button>
          </template>
        </el-table-column>
      </el-table>
    </div>
  `,
  setup() {
    const channels = ref([]);
    const load = async () => {
      try {
        const data = await api.get('/channels');
        channels.value = data;
      } catch(e) { console.error('[EdgeVision] CamerasView load error:', e); }
    };
    const showAdd = () => ElMessageBox.confirm(
      `<div style="text-align:left;padding:10px 0">
        <label style="display:block;margin-bottom:6px;color:#9ca3af;font-size:13px">Channel ID</label>
        <input id="ev-add-id" style="width:100%;padding:8px 12px;background:#0a0e1a;border:1px solid #1f2937;border-radius:6px;color:#e5e7eb;font-size:14px;outline:none" placeholder="camera-1" />
        <label style="display:block;margin:14px 0 6px;color:#9ca3af;font-size:13px">Name</label>
        <input id="ev-add-name" style="width:100%;padding:8px 12px;background:#0a0e1a;border:1px solid #1f2937;border-radius:6px;color:#e5e7eb;font-size:14px;outline:none" placeholder="Front Door Camera" />
        <label style="display:block;margin:14px 0 6px;color:#9ca3af;font-size:13px">RTSP URL</label>
        <input id="ev-add-url" style="width:100%;padding:8px 12px;background:#0a0e1a;border:1px solid #1f2937;border-radius:6px;color:#e5e7eb;font-size:14px;outline:none" placeholder="rtsp://192.168.1.100:554/stream" />
        <label style="display:block;margin:14px 0 6px;color:#9ca3af;font-size:13px">FPS</label>
        <input id="ev-add-fps" type="number" style="width:100%;padding:8px 12px;background:#0a0e1a;border:1px solid #1f2937;border-radius:6px;color:#e5e7eb;font-size:14px;outline:none" value="5" />
      </div>`,
      'Add Camera',
      { confirmButtonText: 'Add', cancelButtonText: 'Cancel', dangerouslyUseHTMLString: true }
    ).then(async ({ value }) => {
      const cid = document.getElementById('ev-add-id').value;
      const name = document.getElementById('ev-add-name').value || cid;
      const rtsp_url = document.getElementById('ev-add-url').value;
      const fps = parseInt(document.getElementById('ev-add-fps').value) || 5;
      if (!cid) { ElMessage.warning('Channel ID is required'); return; }
      try { await api.post('/channels', { channel_id: cid, name, rtsp_url, fps }); load(); ElMessage.success('Added'); }
      catch(e) { ElMessage.error('Failed'); }
    }).catch(() => {});
    const editChannel = (row) => ElMessageBox.confirm(
      `<div style="text-align:left;padding:10px 0">
        <label style="display:block;margin-bottom:6px;color:#9ca3af;font-size:13px">Name</label>
        <input id="ev-edit-name" style="width:100%;padding:8px 12px;background:#0a0e1a;border:1px solid #1f2937;border-radius:6px;color:#e5e7eb;font-size:14px;outline:none" value="${row.name}" />
        <label style="display:block;margin:14px 0 6px;color:#9ca3af;font-size:13px">RTSP URL</label>
        <input id="ev-edit-url" style="width:100%;padding:8px 12px;background:#0a0e1a;border:1px solid #1f2937;border-radius:6px;color:#e5e7eb;font-size:14px;outline:none" value="${row.rtsp_url || ''}" />
        <label style="display:block;margin:14px 0 6px;color:#9ca3af;font-size:13px">FPS</label>
        <input id="ev-edit-fps" type="number" style="width:100%;padding:8px 12px;background:#0a0e1a;border:1px solid #1f2937;border-radius:6px;color:#e5e7eb;font-size:14px;outline:none" value="${row.fps || 5}" />
      </div>`,
      'Edit Camera',
      { confirmButtonText: 'Save', cancelButtonText: 'Cancel', dangerouslyUseHTMLString: true }
    ).then(async ({ value }) => {
      const name = document.getElementById('ev-edit-name').value;
      const rtsp_url = document.getElementById('ev-edit-url').value;
      const fps = parseInt(document.getElementById('ev-edit-fps').value) || 5;
      try { await api.put('/channels/' + row.channel_id, { name, rtsp_url, fps }); load(); ElMessage.success('Updated'); }
      catch(e) { ElMessage.error('Failed'); }
    }).catch(() => {});
    const delChannel = async (id) => { try { await api.delete('/channels/' + id); load(); ElMessage.success('Deleted'); } catch(e) { ElMessage.error('Failed'); } };
    onMounted(load);
    return { channels, showAdd, editChannel, delChannel };
  }
};

const AlertsView = {
  template: `
    <div>
      <h2 style="font-size:22px;font-weight:600;margin-bottom:20px">Alert Center</h2>
      <el-table :data="alerts" stripe style="width:100%">
        <el-table-column prop="alert_id" label="ID" width="100" />
        <el-table-column prop="timestamp" label="Time" width="180" />
        <el-table-column prop="channel_id" label="Channel" width="120" />
        <el-table-column prop="behavior_type" label="Behavior" />
        <el-table-column prop="alert_level" label="Level" width="100">
          <template #default="scope">
            <el-tag :type="scope.row.alert_level==='critical'?'danger':'warning'">{{scope.row.alert_level}}</el-tag>
          </template>
        </el-table-column>
        <el-table-column prop="confidence" label="Confidence" width="100" />
        <el-table-column prop="status" label="Status" width="100">
          <template #default="scope">
            <el-tag :type="scope.row.status==='pending'?'warning':scope.row.status==='confirmed'?'danger':'info'">{{scope.row.status}}</el-tag>
          </template>
        </el-table-column>
        <el-table-column label="Actions" width="250">
          <template #default="scope">
            <el-button size="small" @click="confirmAlert(scope.row.alert_id)">Confirm</el-button>
            <el-button size="small" @click="ignoreAlert(scope.row.alert_id)">Ignore</el-button>
            <el-button size="small" type="danger" @click="delAlert(scope.row.alert_id)">Delete</el-button>
          </template>
        </el-table-column>
      </el-table>
    </div>
  `,
  setup() {
    const alerts = ref([]);
    const load = async () => { try { alerts.value = await api.get('/alerts'); } catch(e) { console.error(e); } };
    const confirmAlert = async (id) => { try { await api.put('/alerts/' + id + '/status', { status: 'confirmed' }); load(); ElMessage.success('Confirmed'); } catch(e) { ElMessage.error('Failed'); } };
    const ignoreAlert = async (id) => { try { await api.put('/alerts/' + id + '/status', { status: 'ignored' }); load(); ElMessage.success('Ignored'); } catch(e) { ElMessage.error('Failed'); } };
    const delAlert = async (id) => { try { await api.delete('/alerts/' + id); load(); ElMessage.success('Deleted'); } catch(e) { ElMessage.error('Failed'); } };
    onMounted(load);
    return { alerts, confirmAlert, ignoreAlert, delAlert };
  }
};

const SettingsView = {
  template: `
    <div>
      <h2 style="font-size:22px;font-weight:600;margin-bottom:20px">System Settings</h2>
      <el-card>
        <el-form label-width="120px">
          <el-form-item label="Database Path"><el-input v-model="form.db_path" /></el-form-item>
          <el-form-item label="Log Directory"><el-input v-model="form.log_dir" /></el-form-item>
          <el-form-item label="API Port"><el-input-number v-model="form.api_port" /></el-form-item>
          <el-form-item><el-button type="primary" @click="save">Save</el-button></el-form-item>
        </el-form>
      </el-card>
    </div>
  `,
  setup() {
    const form = reactive({ db_path: './data/edgevision.db', log_dir: './logs', api_port: 8080 });
    const save = () => { ElMessage.success('Settings saved'); };
    return { form, save };
  }
};


// DEBUG: Adding missing view components
console.log('[EdgeVision] Defining MonitorView');
console.log('[EdgeVision] Defining AlgorithmsView');
console.log('[EdgeVision] Defining BehaviorsView');

const MonitorView = {
  template: `
    <div>
      <h2 style="font-size:22px;font-weight:600;margin-bottom:20px">Real-time Monitor</h2>
      <el-row :gutter="16">
        <el-col :span="8" v-for="ch in channels" :key="ch.channel_id">
          <el-card shadow="hover">
            <template #header><div style="display:flex;justify-content:space-between;align-items:center"><span>{{ch.name}}</span><el-tag :type="ch.status==='running'?'success':'info'" size="small">{{ch.status}}</el-tag></div></template>
            <div style="position:relative;background:#000;height:200px;overflow:hidden">
              <img :src="streamSrc(ch)" style="width:100%;height:100%;object-fit:cover" />
              <div style="position:absolute;top:8px;right:8px;background:rgba(0,0,0,0.6);color:#f56c6c;padding:2px 8px;border-radius:4px;font-size:12px;display:flex;align-items:center;gap:4px">
                <span style="width:8px;height:8px;background:#f56c6c;border-radius:50%;animation:pulse 1s infinite"></span>
                LIVE
              </div>
            </div>
            <div style="margin-top:12px;display:flex;justify-content:space-between;color:#9ca3af;font-size:13px">
              <span>FPS: {{ch.fps}}</span><span>ID: {{ch.channel_id}}</span>
            </div>
          </el-card>
        </el-col>
      </el-row>
    </div>
  `,
  setup() {
    const channels = ref([]);
    const streamSrc = (ch) => '/api/channels/' + ch.channel_id + '/snapshot?t=' + Date.now();
    const load = async () => {
      try {
        channels.value = await api.get('/channels');
        console.log('[EdgeVision] Monitor loaded', channels.value.length, 'channels');
      } catch(e) { console.error('[EdgeVision] Monitor load error:', e); }
    };
    onMounted(load);
    return { channels, streamSrc };
  }
};

const AlgorithmsView = {
  template: `
    <div>
      <h2 style="font-size:22px;font-weight:600;margin-bottom:20px">Algorithm Models</h2>
      <el-row :gutter="16">
        <el-col :span="8" v-for="m in models" :key="m.model_id">
          <el-card shadow="hover">
            <template #header><div style="display:flex;justify-content:space-between;align-items:center"><span>{{m.name}}</span><el-tag :type="m.active?'success':'info'" size="small">{{m.active?'Active':'Inactive'}}</el-tag></div></template>
            <div style="color:#9ca3af;font-size:13px;line-height:2">
              <div>File: <span style="color:#e5e7eb">{{m.file_path}}</span></div>
              <div>Input: {{m.input_width}}x{{m.input_height}}</div>
              <div>Confidence: {{m.conf_threshold}}</div>
              <div>Quantization: {{m.quantization}}</div>
              <div>Classes: {{m.classes}}</div>
            </div>
          </el-card>
        </el-col>
      </el-row>
    </div>
  `,
  setup() {
    const models = ref([]);
    const load = async () => {
      try { models.value = await api.get('/models'); console.log('[EdgeVision] Algorithms loaded', models.value.length, 'models'); }
      catch(e) { console.error('[EdgeVision] Algorithms load error:', e); }
    };
    onMounted(load);
    return { models };
  }
};

const BehaviorsView = {
  template: `
    <div>
      <h2 style="font-size:22px;font-weight:600;margin-bottom:20px">Behavior Rules</h2>
      <el-table :data="rules" style="width:100%" border>
        <el-table-column prop="rule_id" label="Rule ID" width="180" />
        <el-table-column prop="name" label="Name" />
        <el-table-column label="Conditions" min-width="200">
          <template #default="{row}"><span style="color:#9ca3af">{{JSON.stringify(row.conditions)}}</span></template>
        </el-table-column>
        <el-table-column label="Action" min-width="200">
          <template #default="{row}"><span style="color:#9ca3af">{{JSON.stringify(row.action)}}</span></template>
        </el-table-column>
        <el-table-column label="Priority" width="100">
          <template #default="{row}"><el-tag size="small">{{row.priority}}</el-tag></template>
        </el-table-column>
        <el-table-column label="Enabled" width="100">
          <template #default="{row}"><el-switch v-model="row.enabled" @change="updateRule(row)" /></template>
        </el-table-column>
      </el-table>
    </div>
  `,
  setup() {
    const rules = ref([]);
    const load = async () => {
      try { rules.value = await api.get('/behaviors'); console.log('[EdgeVision] Behaviors loaded', rules.value.length, 'rules'); }
      catch(e) { console.error('[EdgeVision] Behaviors load error:', e); }
    };
    const updateRule = async (row) => {
      try {
        await api.put('/behaviors/' + row.rule_id, { enabled: row.enabled, priority: row.priority });
        ElMessage.success('Updated');
      } catch(e) { ElMessage.error('Failed'); }
    };
    onMounted(load);
    return { rules, updateRule };
  }
};

const MainLayout = {
  template: `
    <el-container style="height:100%;display:flex">
      <el-aside style="width:220px;background:#0d1117;border-right:1px solid #1f2937;padding:20px 0;overflow:hidden">
        <div style="text-align:center;padding:10px 0;border-bottom:1px solid #1f2937">
          <el-icon :size="32" color="#409EFF"><VideoCamera /></el-icon>
          <div style="color:#409EFF;font-weight:bold;font-size:18px">EdgeVision</div>
        </div>
        <el-menu :default-active="activeRoute" background-color="#0d1117" text-color="#9ca3af" active-text-color="#409EFF" router style="border:none;height:calc(100vh - 60px);overflow-y:auto">
          <el-menu-item index="/dashboard"><el-icon><DataBoard /></el-icon><span>Dashboard</span></el-menu-item>
          <el-menu-item index="/cameras"><el-icon><Camera /></el-icon><span>Cameras</span></el-menu-item>
          <el-menu-item index="/alerts"><el-icon><Bell /></el-icon><span>Alerts</span></el-menu-item>
          <el-menu-item index="/settings"><el-icon><Setting /></el-icon><span>Settings</span></el-menu-item>
          <el-menu-item index="/monitor"><el-icon><Monitor /></el-icon><span>Real-time Monitor</span></el-menu-item>
          <el-menu-item index="/algorithms"><el-icon><Cpu /></el-icon><span>Algorithm Management</span></el-menu-item>
          <el-menu-item index="/behaviors"><el-icon><Operation /></el-icon><span>Behavior Rules</span></el-menu-item>
        </el-menu>
      </el-aside>
      <el-container style="flex:1;display:flex;flex-direction:column">
        <el-header style="background:#111827;border-bottom:1px solid #1f2937;display:flex;align-items:center;justify-content:space-between;padding:0 20px">
          <span style="color:#9ca3af">EdgeVision - Video Behavior Analysis</span>
          <div style="display:flex;align-items:center;gap:12px">
            <span style="color:#9ca3af">{{auth.username || 'Admin'}}</span>
            <el-button size="small" @click="handleLogout">Logout</el-button>
          </div>
        </el-header>
        <el-main style="background:#0a0e1a;padding:20px"><router-view /></el-main>
      </el-container>
    </el-container>
  `,
  setup() {
    const auth = useAuth();
    const route = useRoute();
    const activeRoute = computed(() => route.path);
    const handleLogout = () => { auth.logout(); useRouter().push('/login'); };
    return { auth, activeRoute, handleLogout, route };
  }
};

const routes = [
  { path: '/login', component: LoginView },
  { path: '/', component: MainLayout, redirect: '/dashboard', children: [
    { path: 'dashboard', key: true, component: DashboardView },
    { path: 'cameras', key: true, component: CamerasView },
    { path: 'alerts', key: true, component: AlertsView },
    { path: 'settings', key: true, component: SettingsView },
    { path: 'monitor', key: true, component: MonitorView },
    { path: 'algorithms', key: true, component: AlgorithmsView },
    { path: 'behaviors', key: true, component: BehaviorsView },
  ]}
];
// DEBUG: Creating router with routes
console.log('[EdgeVision] Routes defined:', routes.map(r => r.path));
console.log('[EdgeVision] Child routes:', routes[1]?.children?.map(r => r.path));
const router = createRouter({ history: createWebHashHistory(), routes });
console.log('[EdgeVision] Router created');

router.beforeEach((to, from, next) => {
  const token = localStorage.getItem('ev_token');
  if (to.path === '/login' && token) next('/dashboard');
  else if (to.path !== '/login' && !token) next('/login');
  else next();
});

const app = createApp({ template: "<router-view/>" });
const pinia = createPinia();
app.use(pinia);
app.use(ElementPlus);
app.use(router);

for (const [key, comp] of Object.entries(icons)) {
  app.component(key, comp);
}

// DEBUG: Mounting app
console.log('[EdgeVision] Mounting app to #app');
app.mount('#app');
console.log('[EdgeVision] App mounted successfully');

