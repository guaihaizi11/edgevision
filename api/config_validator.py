#!/usr/bin/env python3
\"\"\"EdgeVision Configuration Validator

Validates YAML configuration files for correctness and completeness.
Supports UTF-8 encoded filenames and values.
\"\"\"

import yaml
import json
import os
import sys
from typing import Any, Dict, List, Optional, Tuple


class ConfigValidator:
    \"\"\"Validates EdgeVision configuration files.\"\"\"

    REQUIRED_CHANNEL_FIELDS = ['id', 'name', 'rtsp_url', 'fps', 'enabled']
    REQUIRED_BEHAVIOR_FIELDS = ['name', 'conditions', 'alert_level', 'enabled']
    VALID_ALERT_LEVELS = ['info', 'warning', 'critical']
    VALID_FPS_RANGE = (1, 60)

    def __init__(self, config_dir: str = '.'):
        self.config_dir = config_dir
        self.errors: List[str] = []
        self.warnings: List[str] = []

    def validate_channels(self, filepath: str = 'config/channels.yaml') -> Tuple[bool, List[Dict]]:
        \"\"\"Validate channels configuration file.\"\"\"
        full_path = os.path.join(self.config_dir, filepath)
        if not os.path.exists(full_path):
            self.errors.append(f'Channels config not found: {filepath}')
            return False, []

        try:
            with open(full_path, 'r', encoding='utf-8') as f:
                data = yaml.safe_load(f)
        except yaml.YAMLError as e:
            self.errors.append(f'YAML parse error in {filepath}: {e}')
            return False, []

        channels = data.get('channels', []) if isinstance(data, dict) else data
        validated = []

        for i, ch in enumerate(channels):
            for field in self.REQUIRED_CHANNEL_FIELDS:
                if field not in ch:
                    self.errors.append(f'Channel {i}: missing required field {field}')

            if 'fps' in ch:
                if not isinstance(ch['fps'], int) or ch['fps'] < self.VALID_FPS_RANGE[0] or ch['fps'] > self.VALID_FPS_RANGE[1]:
                    self.warnings.append(f'Channel {i}: FPS {ch.get(ps)} out of recommended range {self.VALID_FPS_RANGE}')

            if 'rtsp_url' in ch:
                if not ch['rtsp_url'].startswith(('rtsp://', 'rtsps://')):
                    self.warnings.append(f'Channel {i}: rtsp_url may be invalid')

            validated.append(ch)

        return len(self.errors) == 0, validated

    def validate_behaviors(self, filepath: str = 'config/behaviors.yaml') -> Tuple[bool, List[Dict]]:
        \"\"\"Validate behaviors configuration file.\"\"\"
        full_path = os.path.join(self.config_dir, filepath)
        if not os.path.exists(full_path):
            self.errors.append(f'Behaviors config not found: {filepath}')
            return False, []

        try:
            with open(full_path, 'r', encoding='utf-8') as f:
                data = yaml.safe_load(f)
        except yaml.YAMLError as e:
            self.errors.append(f'YAML parse error in {filepath}: {e}')
            return False, []

        behaviors = data.get('behaviors', []) if isinstance(data, dict) else data
        validated = []

        for i, beh in enumerate(behaviors):
            for field in self.REQUIRED_BEHAVIOR_FIELDS:
                if field not in beh:
                    self.errors.append(f'Behavior {i}: missing required field {field}')

            if 'alert_level' in beh:
                if beh['alert_level'] not in self.VALID_ALERT_LEVELS:
                    self.warnings.append(f'Behavior {i}: unknown alert_level {beh[lert_level]}')

            validated.append(beh)

        return len(self.errors) == 0, validated

    def validate_system(self, filepath: str = 'config/system.yaml') -> Tuple[bool, Dict]:
        \"\"\"Validate system configuration file.\"\"\"
        full_path = os.path.join(self.config_dir, filepath)
        if not os.path.exists(full_path):
            self.errors.append(f'System config not found: {filepath}')
            return False, {}

        try:
            with open(full_path, 'r', encoding='utf-8') as f:
                data = yaml.safe_load(f)
        except yaml.YAMLError as e:
            self.errors.append(f'YAML parse error in {filepath}: {e}')
            return False, {}

        return len(self.errors) == 0, data or {}

    def run_all(self) -> bool:
        \"\"\"Run all validations.\"\"\"
        self.errors = []
        self.warnings = []

        ok1, _ = self.validate_channels()
        ok2, _ = self.validate_behaviors()
        ok3, _ = self.validate_system()

        all_ok = ok1 and ok2 and ok3

        print('=' * 50)
        print('EdgeVision Configuration Validation')
        print('=' * 50)

        if self.errors:
            print(f'\\nErrors ({len(self.errors)}):')
            for err in self.errors:
                print(f'  ERROR: {err}')

        if self.warnings:
            print(f'\\nWarnings ({len(self.warnings)}):')
            for warn in self.warnings:
                print(f'  WARNING: {warn}')

        print(f'\\nResult: {"PASS" if all_ok else "FAIL"}')
        print(f'Errors: {len(self.errors)}, Warnings: {len(self.warnings)}')

        return all_ok


if __name__ == '__main__':
    config_dir = sys.argv[1] if len(sys.argv) > 1 else '.'
    validator = ConfigValidator(config_dir)
    success = validator.run_all()
    sys.exit(0 if success else 1)
