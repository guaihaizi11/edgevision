#!/usr/bin/env python3
import yaml
import json
import os
from typing import Any, Dict, Optional

DEFAULT_ENCODING = 'utf-8'

class ConfigManager:
    def __init__(self, base_dir='.'):
        self.base_dir = base_dir

    def load_yaml(self, filename, encoding=None):
        filepath = os.path.join(self.base_dir, filename)
        enc = encoding or DEFAULT_ENCODING
        with open(filepath, 'r', encoding=enc) as f:
            return yaml.safe_load(f)

    def save_yaml(self, filename, data, encoding=None):
        filepath = os.path.join(self.base_dir, filename)
        enc = encoding or DEFAULT_ENCODING
        with open(filepath, 'w', encoding=enc) as f:
            yaml.dump(data, f, allow_unicode=True, default_flow_style=False)

    def load_json(self, filename, encoding=None):
        filepath = os.path.join(self.base_dir, filename)
        enc = encoding or DEFAULT_ENCODING
        with open(filepath, 'r', encoding=enc) as f:
            return json.load(f)

    def save_json(self, filename, data, encoding=None):
        filepath = os.path.join(self.base_dir, filename)
        enc = encoding or DEFAULT_ENCODING
        with open(filepath, 'w', encoding=enc) as f:
            json.dump(data, f, ensure_ascii=False, indent=2)
