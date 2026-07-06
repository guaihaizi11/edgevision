# EdgeVision Web Frontend

## Technology Stack
- **Vue 3** + Composition API
- **Element Plus** UI component library
- **ECharts** data visualization
- **Pinia** state management
- **Vue Router** routing
- **Axios** HTTP client

## Development
`ash
npm install
npm run dev
`

## Build
`ash
npm run build
`

## Page Routing
| Route | Page | Description |
|------|------|------|
| /login | Login | User authentication |
| /dashboard | Dashboard | Overview statistics |
| /monitor | Real-time Monitor | Multi-channel video preview |
| /cameras | Camera Management | Channel CRUD |
| /algorithms | Algorithm Management | Model management |
| /behaviors | Behavior Rules | Rule configuration |
| /alerts | Alert Center | Alert list/details |
| /settings | System Settings | Users/configuration |

## API Proxy
During development, Vite proxy forwards /api requests to the backend FastAPI service (default port 8080).
