class_name Lifebar
extends Node3D

@export var progressBar: ProgressBar

func set_health(value: float, max_value: float) -> void:
	progressBar.max_value = max_value
	progressBar.value = value
